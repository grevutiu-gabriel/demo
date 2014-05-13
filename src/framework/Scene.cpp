#include "Scene.h"
#include <util/Path.h>


#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QStringList>
#include <QDebug>
#include <QVariantMap>






void Scene::load( const std::string& filename )
{
	m_filename = filename;
	std::cout << "Scene::load " << filename << std::endl;
	//houdini::HouGeoIO::makeLog(filename, &std::cout);

	std::ifstream in( base::expand(filename).c_str(), std::ios_base::in | std::ios_base::binary );
	houdini::json::JSONLogger logger(std::cout);
	houdini::json::JSONReader reader;
	houdini::json::Parser p;
	//p.parse( &in, &logger );
	p.parse( &in, &reader );


	//
	houdini::json::ObjectPtr root = reader.getRoot().asObject();
	if(root && root->hasKey("locators"))
	{
		houdini::json::ObjectPtr locators = root->getObject("locators");
		std::vector<std::string> keys;
		locators->getKeys(keys);

		for(auto it = keys.begin(), end=keys.end();it!=end;++it)
		{
			std::string name = *it;
			houdini::json::ObjectPtr locator = locators->getObject(name);
			loadLocator(locator, "/obj/" + name);
		}
	}
	if(root && root->hasKey("cameras"))
	{
		houdini::json::ObjectPtr cameras = root->getObject("cameras");

		std::vector<std::string> keys;
		cameras->getKeys(keys);

		for(auto it = keys.begin(), end=keys.end();it!=end;++it)
		{
			std::string name = *it;
			houdini::json::ObjectPtr camera = cameras->getObject(name);
			loadCamera(camera, "/obj/" + name);
		}
	}
	if(root && root->hasKey("switchers"))
	{
		houdini::json::ObjectPtr switchers = root->getObject("switchers");

		std::vector<std::string> keys;
		switchers->getKeys(keys);

		for(auto it = keys.begin(), end=keys.end();it!=end;++it)
		{
			std::string name = *it;
			houdini::json::ObjectPtr switcher = switchers->getObject(name);
			loadSwitcher(switcher, "/obj/" + name);
		}
	}
	if(root && root->hasKey("channels"))
	{
		houdini::json::ObjectPtr channels = root->getObject("channels");

		std::vector<std::string> keys;
		channels->getKeys(keys);

		for(auto it = keys.begin(), end=keys.end();it!=end;++it)
		{
			std::string name = *it;
			houdini::json::ObjectPtr channel = channels->getObject(name);
			loadChannel(channel, "/ch/" + name);
		}
	}

//	//get content from json object
//	Camera::Ptr cam1 = Camera::create();
//	cam1->xformMatrix = ConstantM44fController::create( math::M44f::TranslationMatrix(0.0f, 1.0f, 2.0f) );
//	cam1->fov = ConstantFloatController::create( 54.0f );
//	cam1->aspect = ConstantFloatController::create( 1.0f );
//	cam1->projectionMatrix = ProjectionMatrixController::create( cam1->fov, cam1->aspect );
//	m_cameras["cam1"] = cam1;

	// add some dummy channel
	//m_channels["color"] = FloatToV3fControllerOld::create( ConstantFloatController::create(1.0f), SinusController::create(), ConstantFloatController::create(0.0f) );
}

const std::string &Scene::getFilename() const
{
	return m_filename;
}

void Scene::serialize(Serializer &out)
{
	Object::serialize(out);
	out.write("filename", m_filename);
}

//{


//	// open new file
//	QFile file;
//	file.setFileName(filename);
//	file.open(QIODevice::ReadOnly | QIODevice::Text);

//	QJsonDocument sd = QJsonDocument::fromJson(file.readAll());
//	QJsonObject root = sd.object();

//	// cameras ---
//	if( root.contains("cameras") )
//	{
//		QJsonObject cameras = root.value("cameras").toObject();
//		for( auto it = cameras.begin(), end = cameras.end(); it != end; ++it )
//			m_cameras[it.key()] = loadCamera( it.value().toObject() );
//	}

	/*
	// locators ---
	if( root.contains("locators") )
	{
		QJsonObject locators = root.value("locators").toObject();
		for( auto it = locators.begin(), end = locators.end(); it != end; ++it )
			m_cameras[it.key()] = loadCamera( it.value().toObject() );
	}
	*/
//}

FloatController::Ptr Scene::loadTrack( houdini::json::ObjectPtr track, const std::string& name )
{
	float fps = 24.0f;
	int numSamples = track->get<int>("nsamples");
	houdini::json::ArrayPtr data = track->getArray("data");
	base::PiecewiseLinearFunction<float> plf;
	for(int i=0;i<numSamples;++i)
		plf.addSample(float(i)/fps, data->get<float>(i));
	CurveFloatController::Ptr curveController = CurveFloatController::create( plf );
	m_controller[name] = curveController;
	return curveController;
}

M44fController::Ptr Scene::loadTransform( houdini::json::ObjectPtr transform, const std::string&name )
{
	PRSController::Ptr transformController = PRSController::create();
	houdini::json::ObjectPtr channels = transform->getObject("channels");
	{
		bool hasAnimatedX = channels->hasKey("transform.tx");
		bool hasAnimatedY = channels->hasKey("transform.ty");
		bool hasAnimatedZ = channels->hasKey("transform.tz");
		FloatController::Ptr translationX, translationY, translationZ;
		if(hasAnimatedX)
			translationX = loadTrack( channels->getObject("transform.tx"), name + ".tx" );
		else
		if( transform->hasKey("transform.tx") )
			translationX = ConstantFloatController::create(transform->get<float>("transform.tx"));
		else
			translationX = ConstantFloatController::create(0.0f);
		m_controller[name + ".tx"] = translationX;
		m_updateGraph.addConnection( translationX, transformController, "tx" );

		if(hasAnimatedY)
			translationY = loadTrack( channels->getObject("transform.ty"), name + ".ty" );
		else
		if( transform->hasKey("transform.ty") )
			translationY = ConstantFloatController::create(transform->get<float>("transform.ty"));
		else
			translationY = ConstantFloatController::create(0.0f);
		m_controller[name + ".ty"] = translationY;
		m_updateGraph.addConnection( translationY, transformController, "ty" );

		if(hasAnimatedZ)
			translationZ = loadTrack( channels->getObject("transform.tz"), name + ".tz" );
		else
		if( transform->hasKey("transform.tz") )
			translationZ = ConstantFloatController::create(transform->get<float>("transform.tz"));
		else
			translationZ = ConstantFloatController::create(0.0f);
		m_controller[name + ".tz"] = translationZ;
		m_updateGraph.addConnection( translationZ, transformController, "tz" );
		//translation = FloatToV3fControllerOld::create( translationX, translationY, translationZ );
		//m_controller[name + "transform.translation"] = translation;
	}
	V3fController::Ptr rotation;
	{
		bool hasAnimatedX = channels->hasKey("transform.rx");
		bool hasAnimatedY = channels->hasKey("transform.ry");
		bool hasAnimatedZ = channels->hasKey("transform.rz");
		FloatController::Ptr rotationX, rotationY, rotationZ;
		if(hasAnimatedX)
			rotationX = loadTrack( channels->getObject("transform.rx"), name + ".rx" );
		else
			rotationX = ConstantFloatController::create(0.0f);
		m_controller[name + ".rx"] = rotationX;
		m_updateGraph.addConnection( rotationX, transformController, "rx" );
		if(hasAnimatedY)
			rotationY = loadTrack( channels->getObject("transform.ry"), name + "transform.ry" );
		else
			rotationY = ConstantFloatController::create(0.0f);
		m_controller[name + ".ry"] = rotationY;
		m_updateGraph.addConnection( rotationY, transformController, "ry" );
		if(hasAnimatedZ)
			rotationZ = loadTrack( channels->getObject("transform.rz"), name + ".rz" );
		else
			rotationZ = ConstantFloatController::create(0.0f);
		m_controller[name + ".rz"] = rotationZ;
		m_updateGraph.addConnection( rotationZ, transformController, "rz" );
	}
	{
		FloatController::Ptr scaleX, scaleY, scaleZ;
		scaleX = ConstantFloatController::create(1.0f);
		scaleY = ConstantFloatController::create(1.0f);
		scaleZ = ConstantFloatController::create(1.0f);
		m_controller[name + ".sx"] = scaleX;
		m_updateGraph.addConnection( scaleX, transformController, "sx" );
		m_controller[name + ".sy"] = scaleY;
		m_updateGraph.addConnection( scaleY, transformController, "sy" );
		m_controller[name + ".sz"] = scaleZ;
		m_updateGraph.addConnection( scaleZ, transformController, "sz" );
	}
	//V3fController::Ptr scale = ConstantV3fController::create(math::V3f(1.0f));
	//return PRSController::create( translation, rotation, scale );
	m_controller[name] = transformController;
	return transformController;
}

M44fController::Ptr Scene::loadLocator( houdini::json::ObjectPtr transform, const std::string& name )
{
	return loadTransform( transform, name + "/transform" );
}

CameraController::Ptr Scene::loadCamera( houdini::json::ObjectPtr camera, const std::string& name )
{
	PerspectiveCameraController::Ptr cam = PerspectiveCameraController::create();

	math::V2i res(int(camera->getValue("resx").as<float>()), int(camera->getValue("resy").as<float>()));
	float asp = 1.0f; // pixel aspect
	float apx = camera->getValue("camera.horizontalFilmAperture").as<float>();
	float focal = 50.0f;

	if( camera->hasKey("camera.fl") )
		focal = camera->getValue("camera.fl").as<float>();

	float apy = (res.y*apx) / (res.x*asp);
	float fovx = 2*atan( (apx/2) / focal );
	float fovy = 2*atan( (apy/2) / focal );

	ConstantFloatController::Ptr fovController = ConstantFloatController::create( fovy );
	m_controller[name + "/fovy"] = fovController;
	ConstantFloatController::Ptr aspectController = ConstantFloatController::create( fovx/fovy );
	m_controller[name + "/aspect"] = aspectController;
	M44fController::Ptr transformController = loadTransform( camera, name + "/transform");
	m_controller[name] = cam;

	// set connections
	m_updateGraph.addConnection( fovController, cam, "fovy" );
	m_updateGraph.addConnection( aspectController, cam, "aspect" );
	m_updateGraph.addConnection( transformController, cam, "transform" );

	return cam;
}

CameraController::Ptr Scene::loadSwitcher( houdini::json::ObjectPtr switcher, const std::string& name )
{
	CameraSwitchController::Ptr s = CameraSwitchController::create();
	houdini::json::ObjectPtr channels = switcher->getObject("channels");

	if(channels->hasKey("camswitch"))
	{
		loadTrack( channels->getObject("camswitch"), name + ".camswitch" );
	}else
	{
		m_controller[name + ".camswitch"] = ConstantFloatController::create( 0.0f );
	}

	if(switcher->hasKey("cameras"))
	{
		houdini::json::ArrayPtr cameras = switcher->getArray("cameras");
		int numItems = cameras->size();
		for( int i=0;i<numItems;++i )
		{
			//TODO
//			std::string cameraName = cameras->get<std::string>(i);
//			CameraController::Ptr cam = getCamera( cameraName );
//			if( cam )
//				s->m_cameras.push_back( cam );
		}
	}

	m_controller[name] = s;
	return s;
}

void Scene::loadChannel( houdini::json::ObjectPtr channel, const std::string& channelName )
{
	std::vector<std::string> tracks;
	channel->getKeys(tracks);
	if( int(tracks.size()) == 1 )
	{
		const std::string& trackName = tracks[0];
		std::cout << "loading " << channelName << std::endl;
		FloatController::Ptr track = loadTrack( channel->getObject(trackName), channelName );
		m_controller[channelName] = track;
		m_controller[channelName + "." + trackName] = track;
	}else
	{
		// multi track channel
		for(auto it = tracks.begin(),end=tracks.end();it!=end;++it)
		{
			const std::string& trackName = *it;
			std::string name = channelName + "." + trackName;
			std::cout << "loading " << name << std::endl;
			FloatController::Ptr track = loadTrack( channel->getObject(trackName), channelName + "." + trackName );
			m_controller[name] = track;
		}
	}
}

/*
Camera::Ptr HouScene::loadCamera( QJsonObject &obj )
{
	//http://www.sidefx.com/docs/houdini12.0/ref/cameralenses

	math::V2i res(int(obj.value("resx").toDouble()), int(obj.value("resy").toDouble())); // pixel resolution
	float asp = 1.0f; // pixel aspect
	float apx = obj.value("camera.horizontalFilmAperture").toDouble();
	float focal = 50.0f;

	if( obj.value("camera.fl").isDouble() )
		focal = obj.value("camera.fl").toDouble();
	else
	if( obj.value("camera.fl").isString() )
		focal = obj.value("camera.fl").toString().toFloat();

	float apy = (res.y*apx) / (res.x*asp);
	float fovx = 2*atan( (apx/2) / focal );
	float fovy = 2*atan( (apy/2) / focal );

	Camera::Ptr camera = std::make_shared<Camera>( fovy, fovx/fovy );
	camera->setRaster( res.x, res.y, float(res.x)/float(res.y) );

	math::M44f translation = math::M44f::TranslationMatrix(obj.value("transform.tx").toDouble(), obj.value("transform.ty").toDouble(), obj.value("transform.tz").toDouble());
	math::M44f rotationX = math::M44f::RotationMatrixX( -math::degToRad(obj.value("transform.rx").toDouble()) );
	math::M44f rotationY = math::M44f::RotationMatrixY( -math::degToRad(obj.value("transform.ry").toDouble()) );
	math::M44f rotationZ = math::M44f::RotationMatrixZ( -math::degToRad(obj.value("transform.rz").toDouble()) );

	camera->setViewToWorld( rotationX*rotationY*rotationZ*translation );
	return camera;
}
*/


void SceneController::update(Property::Ptr prop, float time)
{
	std::cout << "SceneController::update " << m_controllerId << " " << prop->getName() << std::endl;

	// since the controller may be driven by other controllers, we have an updategraph
	m_updateGraph.update( time );
	// now, due to the udate graph, m_controller is supposed to be up to date...
	m_controller->update( prop, time );
}

void SceneController::serialize(Serializer &out)
{
	Controller::serialize(out);
	out.write( "scene", out.serialize(m_scene) );
	out.write( "controller", m_controllerId );
}

void SceneController::getController()
{
	// get Controller from scene
	m_controller = m_scene->getController(m_controllerId, m_updateGraph);
	m_updateGraph.compile();
}




REGISTERCLASS( SceneController )
REGISTERCLASS( Scene )
