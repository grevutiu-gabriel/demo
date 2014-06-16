#include "Scene.h"
#include <util/Path.h>
#include <util/StringManip.h>


#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QStringList>
#include <QDebug>
#include <QVariantMap>

#include "elements/volumept/TransferFunction.h"





bool Scene::load( const std::string& filename )
{
	m_filename = filename;
	std::cout << "Scene::load " << filename << std::endl;
	//houdini::HouGeoIO::makeLog(filename, &std::cout);

	std::ifstream in( base::expand(filename).c_str(), std::ios_base::in  | std::ios::binary);

	if(!in)
		return false;

	int contentsSize = 0;
	{
		in.seekg(0, std::ios::end);
		contentsSize = in.tellg();
		in.seekg(0, std::ios::beg);
	}

	if(!contentsSize)
		return false;

	houdini::json::JSONLogger logger(std::cout);
	houdini::json::JSONReader reader;
	houdini::json::Parser p;
	//p.parse( &in, &logger );
	p.parse( &in, &reader );

	//if(!m_reloading)
	{

	//
	houdini::json::ObjectPtr root = reader.getRoot().asObject();

	// scene info
	if(root && root->hasKey("info"))
	{
		houdini::json::ObjectPtr info = root->getObject("info");
		m_fps = 24.0f;
		if(info->hasKey("startFrame"))
		{
			m_startTime = float(info->get<int>("startFrame"))/m_fps;
		}
		if(info->hasKey("endFrame"))
		{
			m_endTime = float(info->get<int>("endFrame"))/m_fps;
		}
	}

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
	if(root && root->hasKey("geometries"))
	{
		houdini::json::ObjectPtr geometries = root->getObject("geometries");
		std::vector<std::string> keys;
		geometries->getKeys(keys);

		for(auto it = keys.begin(), end=keys.end();it!=end;++it)
		{
			std::string name = *it;
			houdini::json::ObjectPtr geometry = geometries->getObject(name);
			loadGeometry(geometry, "/obj/" + name);
		}
	}
	if(root && root->hasKey("lights"))
	{
		houdini::json::ObjectPtr lights = root->getObject("lights");
		std::vector<std::string> keys;
		lights->getKeys(keys);

		for(auto it = keys.begin(), end=keys.end();it!=end;++it)
		{
			std::string name = *it;
			houdini::json::ObjectPtr light = lights->getObject(name);
			loadLight(light, "/obj/" + name);
		}
	}

	} // reloading

	//if(!m_reloading)
	for( auto it:m_controller )
	{
		//std::cout << it.first << std::endl;
		it.second->setName(it.first);
	}

	return true;
}

const std::string &Scene::getFilename() const
{
	return m_filename;
}

float Scene::getEndTime() const
{
	return m_endTime;
}

void Scene::registerReloadCallback(Scene::ReloadCallback callback)
{
	m_reloadCallbacks.push_back(callback);
}

void Scene::getControllerNames(std::vector<std::string> &names)
{
	names.clear();
	for( auto it = m_controller.begin(), end=m_controller.end();it!=end;++it )
		names.push_back( it->first );
}

void Scene::reload()
{
	m_reloading = true;
	m_updateGraph.clear();
	m_controller.clear();
	bool success = load(m_filename);
	for(auto callback:m_reloadCallbacks)
		callback();
}

void Scene::serialize(Serializer &out)
{
	Object::serialize(out);
	out.write("filename", m_filename);
}

void Scene::deserialize(Deserializer &in)
{
	Object::deserialize(in);
	m_filename = in.readString("filename");
	load(m_filename);
}



FloatController::Ptr Scene::loadTrack( houdini::json::ObjectPtr track, const std::string& name )
{
	float fps = m_fps;
	int numSamples = track->get<int>("nsamples");
	houdini::json::ArrayPtr data = track->getArray("data");
	base::PiecewiseLinearFunction<float> plf;
	for(int i=0;i<numSamples;++i)
		plf.addSample(float(i)/fps, data->get<float>(i));
	CurveFloatController::Ptr curveController = CurveFloatController::create( plf );
	m_controller[name] = curveController;
	return curveController;
}

FloatController::Ptr Scene::loadFloatParameter(houdini::json::ObjectPtr container, const std::string &parmName, const std::string &outName)
{
	FloatController::Ptr controller;
	houdini::json::ObjectPtr channels;

	bool isAnimated = false;

	if( container->hasKey("channels"))
	{
		channels = container->getObject("channels");
		isAnimated = channels->hasKey(parmName);
	}


	if(isAnimated)
		controller = loadTrack( channels->getObject(parmName), outName );
	else
	if( container->hasKey(parmName) )
		controller = ConstantFloatController::create(container->get<float>(parmName));
	else
		controller = ConstantFloatController::create(0.0f);

	m_controller[outName] = controller;

	return controller;
}

bool Scene::hasFloatParameter(houdini::json::ObjectPtr container, const std::string &parmName)
{
	if( container->hasKey(parmName) )
		return true;
	else
	if( container->hasKey("channels"))
	{
		if(container->getObject("channels")->hasKey(parmName))
			return true;
	}
	return false;
}

FloatPLFController::Ptr Scene::loadScalarRamp(houdini::json::ObjectPtr container, const std::string &parmName, const std::string &outName)
{
	FloatPLFController::Ptr controller = std::make_shared<FloatPLFController>();

	int current = 1;
	std::string nextPointName = parmName+base::toString(current);
	while( hasFloatParameter(container, nextPointName+"pos")&&
		   hasFloatParameter(container, nextPointName+"value"))
	{
		FloatController::Ptr posController = loadFloatParameter(container, nextPointName+"pos", outName + "/" + nextPointName+"pos");
		FloatController::Ptr valueController = loadFloatParameter(container, nextPointName+"value", outName + "/" + nextPointName+"value");
		controller->addPoint( posController->evaluate(0.0f), valueController->evaluate(0.0f) );
		m_updateGraph.addConnection( posController, controller, "point"+base::toString(current-1)+".x" );
		m_updateGraph.addConnection( valueController, controller, "point"+base::toString(current-1)+".y" );

		++current;
		nextPointName = parmName+base::toString(current);
	}

	m_controller[outName] = controller;

	return controller;
}

V3fPLFController::Ptr Scene::loadColorRamp(houdini::json::ObjectPtr container, const std::string &parmName, const std::string &outName)
{
	V3fPLFController::Ptr controller = std::make_shared<V3fPLFController>();

	int current = 1;
	std::string nextPointName = parmName+base::toString(current);
	while( hasFloatParameter(container, nextPointName+"pos")&&
		   hasFloatParameter(container, nextPointName+"cr")&&
		   hasFloatParameter(container, nextPointName+"cg")&&
		   hasFloatParameter(container, nextPointName+"cb"))
	{
		FloatController::Ptr posController = loadFloatParameter(container, nextPointName+"pos", outName + "/" + nextPointName+"pos");
		FloatController::Ptr valueRController = loadFloatParameter(container, nextPointName+"cr", outName + "/" + nextPointName+"cr");
		FloatController::Ptr valueGController = loadFloatParameter(container, nextPointName+"cg", outName + "/" + nextPointName+"cg");
		FloatController::Ptr valueBController = loadFloatParameter(container, nextPointName+"cb", outName + "/" + nextPointName+"cb");
		controller->addPoint( posController->evaluate(0.0f), math::V3f(valueRController->evaluate(0.0f),
																	   valueGController->evaluate(0.0f),
																	   valueBController->evaluate(0.0f)) );

		FloatToV3fController::Ptr tovec3 = FloatToV3fController::create();


		m_updateGraph.addConnection( posController, controller, "point"+base::toString(current-1)+".x" );
		m_updateGraph.addConnection( valueRController, tovec3, "x" );
		m_updateGraph.addConnection( valueGController, tovec3, "y" );
		m_updateGraph.addConnection( valueBController, tovec3, "z" );
		m_updateGraph.addConnection( tovec3, controller, "point"+base::toString(current-1)+".value" );

		++current;
		nextPointName = parmName+base::toString(current);
	}

	m_controller[outName] = controller;

	return controller;

}

M44fController::Ptr Scene::loadTransform( houdini::json::ObjectPtr transform, const std::string&name )
{
	PRSController::Ptr transformController = PRSController::create();
	{
		FloatController::Ptr translationX, translationY, translationZ;
		translationX = loadFloatParameter( transform, "transform.tx", name + ".tx" );
		m_updateGraph.addConnection( translationX, transformController, "tx" );
		translationY = loadFloatParameter( transform, "transform.ty", name + ".ty" );
		m_updateGraph.addConnection( translationY, transformController, "ty" );
		translationZ = loadFloatParameter( transform, "transform.tz", name + ".tz" );
		m_updateGraph.addConnection( translationZ, transformController, "tz" );

		FloatToV3fController::Ptr translationController = FloatToV3fController::create();
		m_controller[name + ".translation"] = translationController;
		m_updateGraph.addConnection( translationX, translationController, "x" );
		m_updateGraph.addConnection( translationY, translationController, "y" );
		m_updateGraph.addConnection( translationZ, translationController, "z" );
	}
	{
		FloatController::Ptr rotationX, rotationY, rotationZ;
		rotationX = loadFloatParameter( transform, "transform.rx", name + ".rx" );
		m_updateGraph.addConnection( rotationX, transformController, "rx" );
		rotationY = loadFloatParameter( transform, "transform.ry", name + ".ry" );
		m_updateGraph.addConnection( rotationY, transformController, "ry" );
		rotationZ = loadFloatParameter( transform, "transform.rz", name + ".rz" );
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

void Scene::loadGeometry(houdini::json::ObjectPtr geometry, const std::string &name)
{
	loadTransform( geometry, name + "/transform" );

	// sops
	if(geometry->hasKey("sops"))
	{
		houdini::json::ObjectPtr sops = geometry->getObject("sops");
		std::vector<std::string> keys;
		sops->getKeys(keys);

		for(auto it = keys.begin(), end=keys.end();it!=end;++it)
		{
			std::string sopName = *it;
			houdini::json::ObjectPtr sop = sops->getObject(sopName);
			loadSOP(sop, name + "/" + sopName);
		}
	}
}

void Scene::loadSOP(houdini::json::ObjectPtr sop, const std::string &name)
{
	std::string sopType = sop->get<std::string>("soptype");

	if( sopType == "volumeramp" )
	{
		std::cout << "loading sop: " << name << std::endl;
		FloatPLFController::Ptr scalarRamp = loadScalarRamp( sop, "scalarramp", name + "/scalarramp" );
		V3fPLFController::Ptr colorRamp = loadColorRamp( sop, "colorramp", name + "/colorramp" );
		FloatController::Ptr destMax = loadFloatParameter( sop, "destmax", name + "/destmax" );

		// now we bake animated scalar and color transfer function into
		// a 2d texture (1 dimension for time and the second dimension for scalar and color (V4f))
		int numTimeSamples = 512;
		int numRangeSamples = 256;

		base::Texture2d::Ptr texture = base::Texture2d::createRGBAFloat32( numRangeSamples, numTimeSamples );

		std::vector<math::V4f> textureValues;
		textureValues.resize(numRangeSamples*numTimeSamples);


		float startTime = m_startTime;
		float endTime = m_endTime;

		// get updategraph for our 2 controllers
		UpdateGraph ug;
		ug.copyFrom( m_updateGraph, scalarRamp, colorRamp );

		// time dimension
		for(int i=0;i<numTimeSamples;++i)
		{
			float time = startTime + (float(i)/float(numTimeSamples-1))*(endTime-startTime);

			// update ramps
			ug.update(time);

			// evaluate ramps
			FloatPLFController::PLFPTR density = scalarRamp->evaluate(time);
			V3fPLFController::PLFPTR color = colorRamp->evaluate(time);

			// sample ramps across range dimension
			// which we know, goes from 0 to 1
			for(int j=0;j<numRangeSamples;++j)
			{
				float t = float(j)/float(numRangeSamples-1);
				float newDensity = density->evaluate(t);
				math::V3f newColor = color->evaluate(t);
				textureValues[i*numRangeSamples+j] = math::V4f(newColor.x, newColor.y, newColor.z, newDensity);
				//textureValues[i*numRangeSamples+j] = math::V4f(newDensity, newDensity, newDensity, newDensity);

				//std::cout << "timesample " << i << " time: " << time <<  "   rangeSample: " << j << " density:" << newDensity << std::endl;
			}
		}

		texture->uploadRGBAFloat32( numRangeSamples, numTimeSamples, (float*)&textureValues[0] );

		//ConstantTexture2dController::Ptr temp = ConstantTexture2dController::create(texture);
		//m_controller[name + "/baked"] = temp;
		ConstantAnimatedTransferFunctionController::Ptr catfc = std::make_shared<ConstantAnimatedTransferFunctionController>(std::make_shared<AnimatedTransferFunction>(texture, m_startTime,m_endTime, destMax->evaluate(0.0f)));
		m_controller[name + "/baked"] = catfc;
	}
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

void Scene::loadLight(houdini::json::ObjectPtr light, const std::string &name)
{
	M44fController::Ptr transformController = loadTransform( light, name + "/transform");
	FloatController::Ptr intensityController = loadFloatParameter( light, "light_intensity", name + "/light_intensity" );
	FloatController::Ptr colorRController = loadFloatParameter( light, "light_colorr", name + "/light_colorr" );
	FloatController::Ptr colorGController = loadFloatParameter( light, "light_colorg", name + "/light_colorg" );
	FloatController::Ptr colorBController = loadFloatParameter( light, "light_colorb", name + "/light_colorb" );

	V3fController::Ptr colorController = FloatToV3fController::create();
	m_updateGraph.addConnection( colorRController, colorController, "x" );
	m_updateGraph.addConnection( colorGController, colorController, "y" );
	m_updateGraph.addConnection( colorBController, colorController, "z" );
	m_controller[name+"/light_color"] = colorController;
}

CameraController::Ptr Scene::loadSwitcher( houdini::json::ObjectPtr switcher, const std::string& name )
{
	CameraSwitchController::Ptr s = CameraSwitchController::create();
	houdini::json::ObjectPtr channels = switcher->getObject("channels");

	FloatController::Ptr switchController;
	if(channels->hasKey("camswitch"))
	{
		switchController = loadTrack( channels->getObject("camswitch"), name + ".camswitch" );
	}else
	{
		switchController = ConstantFloatController::create( 0.0f );
	}
	m_controller[name + ".camswitch"] = switchController;

	if(switcher->hasKey("cameras"))
	{
		houdini::json::ArrayPtr cameras = switcher->getArray("cameras");
		int numItems = cameras->size();
		for( int i=0;i<numItems;++i )
		{
			std::string cameraName = cameras->get<std::string>(i);
			CameraSwitchController::Item item;
			item.second = new UpdateGraph();
			item.first = std::dynamic_pointer_cast<CameraController>(getController( "/obj/" + cameraName, *item.second ));
			if( item.first )
			{
				item.second->compile();
				s->m_cameras.push_back( item );
			}
		}
	}

	m_updateGraph.addConnection(switchController, s, "switch");

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


SceneController::SceneController(SceneController::ScenePtr scene, const std::string &controllerId)
	:Controller(),
	 m_scene(scene),
	 m_controllerId(controllerId)
{
	m_scene->registerReloadCallback( std::bind( &SceneController::getController, this ) );
	getController();
}

void SceneController::update(Property::Ptr prop, float time)
{
	if(m_controller)
	{
		//std::cout << "SceneController::update " << m_controllerId << " " << prop->getName() << std::endl;

		// since the controller may be driven by other controllers, we have an updategraph
		m_updateGraph.update( time );
		// now, due to the udate graph, m_controller is supposed to be up to date...
		m_controller->update( prop, time );
	}
}

void SceneController::serialize(Serializer &out)
{
	Controller::serialize(out);
	out.write( "scene", out.serialize(m_scene) );
	out.write( "controller", m_controllerId );
}

void SceneController::deserialize(Deserializer &in)
{
	Controller::deserialize(in);
	m_scene = std::dynamic_pointer_cast<Scene>(in.deserializeObject(in.readValue("scene")));
	m_controllerId = in.readString("controller");
	getController();
}

void SceneController::getController()
{
	// get Controller from scene
	m_controller = m_scene->getController(m_controllerId, m_updateGraph);
	if(!m_controller)
		std::cout << "didnt get " << m_controllerId << std::endl;
	else
		std::cout << "got " << m_controllerId << std::endl;
}




REGISTERCLASS( SceneController )
REGISTERCLASS( Scene )
