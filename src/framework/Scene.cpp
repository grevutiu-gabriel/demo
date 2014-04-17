#include "Scene.h"



#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QStringList>
#include <QDebug>
#include <QVariantMap>


#include "houdini/HouGeoIO.h"



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
	if(root->hasKey("locators"))
	{
		std::cout << "!!\n";
		houdini::json::ObjectPtr locators = root->getObject("locators");

	}






	//get content from json object
	Camera::Ptr cam1 = Camera::create();
	cam1->xformMatrix = ConstantM44fController::create( math::M44f::TranslationMatrix(0.0f, 1.0f, 2.0f) );
	cam1->fov = ConstantFloatController::create( 54.0f );
	cam1->aspect = ConstantFloatController::create( 1.0f );
	cam1->projectionMatrix = ProjectionMatrixController::create( cam1->fov, cam1->aspect );
	m_cameras["cam1"] = cam1;

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
