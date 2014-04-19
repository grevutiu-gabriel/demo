#include "Demo.h"
#include "elements/volumept/Volume.h"
#include "elements/postprocess/PostProcess.h"

void Demo::load( const std::string& filename )
{
	// load scenes
	Scene::Ptr scene = Scene::create();
	scene->load("c:\\projects\\demo\\git\\bin\\data\\untitled.scn");
	// load elements
	// load shots
	Shot::Ptr shot0 = Shot::create( scene->getCamera("cam1") );
	Shot::Ptr shot1 = Shot::create( scene->getCamera("cam2") );
	m_shots.push_back(shot0);
	m_shots.push_back(shot1);

	Element::Ptr black = Clear::create(math::V3f(0.0f, 0.0f, 0.0f));
	Element::Ptr red = Clear::create(math::V3f(1.0f, 0.0f, 0.0f));
	Element::Ptr green = Clear::create(math::V3f(0.0f, 1.0f, 0.0f));

	// ------------
	base::Geometry::Ptr geo = houdini::HouGeoIO::importGeometry("c:\\projects\\demo\\git\\bin\\data\\test.bgeo");
	//base::Shader::Ptr shader = base::Shader::createSimpleConstantShader();
	base::Shader::Ptr shader = base::Shader::loadFromFile( "c:\\projects\\demo\\git\\src\\core\\glsl\\genericShader" );
	shader->setUniform("l", math::V3f(1.0f).normalized());
	shader->setUniform("ka", 0.2f);
	shader->setUniform("ambient", math::V3f(1.0f));
	shader->setUniform("kd", 1.0f);
	shader->setUniform("diffuse", math::V3f(.5f));
	Element::Ptr renderGeo = RenderGeometry::create(geo, shader);

	// volume ---
	Volume::Ptr volume = Volume::create();
	volume->load( "c:\\projects\\demo\\git\\bin\\data\\artifix_resized_moved.bgeo" );

	// init post process -----------
	PostProcess::Ptr post = PostProcess::create();
	post->setHDREnabled(true);
	post->setGlareEnabled(true);
	post->setGlareBlurIterations(2);
	post->setGlareAmount(0.2f);




	// shot 0
	{
		Shot::ShotElement::Ptr se = Shot::ShotElement::create(post);
		se->addChild(black);
		se->addChild(volume)->setController("PointLightPosition", scene->getLocator("null1")->xform->translation);
		shot0->addElement(se);
	}

	// shot 1
	{
		Shot::ShotElement::Ptr se = Shot::ShotElement::create(post);
		se->addChild(black);
		se->addChild(volume)->setController("PointLightPosition", scene->getLocator("null1")->xform->translation);
		//shot1->addElement(black);//->setController("color", scene->getChannel("color"));
		shot1->addElement(se);
	}


	m_duration = 10.0f;
	m_shotIndex.addSample(0.0f, 0);
	m_shotIndex.addSample(5.0f, 1);

}
