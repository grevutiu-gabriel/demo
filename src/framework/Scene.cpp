#include "Scene.h"



#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QStringList>
#include <QDebug>
#include <QVariantMap>






void Scene::load( const std::string& filename )
{
	//houdini::HouGeoIO::makeLog(filename, &std::cout);

	std::ifstream in( filename.c_str(), std::ios_base::in | std::ios_base::binary );
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
			m_locators[name] = loadLocator(locator);
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
			m_cameras[name] = loadCamera(camera);
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
	m_channels["color"] = FloatToV3fController::create( ConstantFloatController::create(1.0f), SinusController::create(), ConstantFloatController::create(0.0f) );
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

FloatController::Ptr Scene::loadChannel( houdini::json::ObjectPtr channel )
{
	float fps = 24.0f;
	int numSamples = channel->get<int>("nsamples");
	houdini::json::ArrayPtr data = channel->getArray("data");
	base::PiecewiseLinearFunction<float> plf;
	for(int i=0;i<numSamples;++i)
		plf.addSample(float(i)/fps, data->get<float>(i));
	return CurveFloatController::create( plf );
}

void Scene::loadTransform( houdini::json::ObjectPtr transform, Transform::Ptr xform )
{
	houdini::json::ObjectPtr channels = transform->getObject("channels");
	V3fController::Ptr translation;
	{
		bool hasX = channels->hasKey("transform.tx");
		bool hasY = channels->hasKey("transform.ty");
		bool hasZ = channels->hasKey("transform.tz");
		if( hasX||hasY||hasZ )
		{
			FloatController::Ptr translationX, translationY, translationZ;
			if(hasX)
				translationX = loadChannel( channels->getObject("transform.tx") );
			else
			if( transform->hasKey("transform.tx") )
				translationX = ConstantFloatController::create(transform->get<float>("transform.tx"));
			else
				translationX = ConstantFloatController::create(0.0f);
			if(hasY)
				translationY = loadChannel( channels->getObject("transform.ty") );
			else
			if( transform->hasKey("transform.ty") )
				translationY = ConstantFloatController::create(transform->get<float>("transform.ty"));
			else
				translationY = ConstantFloatController::create(0.0f);
			if(hasZ)
				translationZ = loadChannel( channels->getObject("transform.tz") );
			else
			if( transform->hasKey("transform.tz") )
				translationZ = ConstantFloatController::create(transform->get<float>("transform.tz"));
			else
				translationZ = ConstantFloatController::create(0.0f);
			translation = FloatToV3fController::create( translationX, translationY, translationZ );
		}else
			translation = ConstantV3fController::create(math::V3f(0.0f));
	}
	V3fController::Ptr rotation;
	{
		bool hasX = channels->hasKey("transform.rx");
		bool hasY = channels->hasKey("transform.ry");
		bool hasZ = channels->hasKey("transform.rz");
		if( hasX||hasY||hasZ )
		{
			FloatController::Ptr rotationX, rotationY, rotationZ;
			if(hasX)
				rotationX = loadChannel( channels->getObject("transform.rx") );
			else
				rotationX = ConstantFloatController::create(0.0f);
			if(hasY)
				rotationY = loadChannel( channels->getObject("transform.ry") );
			else
				rotationY = ConstantFloatController::create(0.0f);
			if(hasZ)
				rotationZ = loadChannel( channels->getObject("transform.rz") );
			else
				rotationZ = ConstantFloatController::create(0.0f);
			rotation = FloatToV3fController::create( rotationX, rotationY, rotationZ );
		}else
			rotation = ConstantV3fController::create(math::V3f(0.0f));
	}
	V3fController::Ptr scale = ConstantV3fController::create(math::V3f(1.0f));
	xform->xform = PRSController::create( translation, rotation, scale );
}

Transform::Ptr Scene::loadLocator( houdini::json::ObjectPtr transform )
{
	Transform::Ptr xform = Transform::create();

	loadTransform( transform, xform );

	return xform;
}

Camera::Ptr Scene::loadCamera( houdini::json::ObjectPtr camera )
{
	houdini::json::ObjectPtr channels = camera->getObject("channels");
	Camera::Ptr cam = Camera::create();

	math::V2i res(int(camera->getValue("resx").as<float>()), int(camera->getValue("resy").as<float>()));
	float asp = 1.0f; // pixel aspect
	float apx = camera->getValue("camera.horizontalFilmAperture").as<float>();
	float focal = 50.0f;

	if( camera->hasKey("camera.fl") )
		focal = camera->getValue("camera.fl").as<float>();

	float apy = (res.y*apx) / (res.x*asp);
	float fovx = 2*atan( (apx/2) / focal );
	float fovy = 2*atan( (apy/2) / focal );

	cam->fov = ConstantFloatController::create( fovy );
	cam->aspect = ConstantFloatController::create( fovx/fovy );
	cam->projectionMatrix = ProjectionMatrixController::create( cam->fov, cam->aspect );

	loadTransform( camera, cam );

	return cam;
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
