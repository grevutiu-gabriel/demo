//============================================================================
//
//
//============================================================================

#define STANDALONE 1


#ifdef STANDALONE
#include <ui/Application.h>
#include <ui/GLViewer.h>
#else
#include <gltools/gl.h>
#include <QtGui>
#include <QApplication>
#endif






#include <gfx/Geometry.h>
#include <gfx/Shader.h>
#include <gfx/Texture.h>
#include <gfx/FBO.h>
#include <util/StopWatch.h>


#include <framework/houdini/HouGeoIO.h>
#include <framework/elements/volumept/Volume.h>
#include <framework/elements/postprocess/PostProcess.h>








#ifdef STANDALONE
base::GLViewer* glviewer;
base::StopWatch timer;
#else

#endif




base::Geometry::Ptr g_geo;
base::Shader::Ptr g_shader;
base::FBO::Ptr g_fbo;
Volume::Ptr g_volume;
PostProcess::Ptr g_post;

float timerMax=-std::numeric_limits<float>::infinity();
float timerAvg=0.0f;
float timerMin=std::numeric_limits<float>::infinity();
int numFrames = 0;



/*
#include <gfx/Context.h>
#include <util/HouGeoIO.h>

#include <ui/GLViewer.h>
#include <widgets/GLViewer/GLViewer.h>
#include <widgets/CategoryList/CategoryList.h>


#include "Audio.h"
#include "Demo.h"




// SCENES ============

#include "scn_nightsky.h"
#include "scn_nebulae1.h"
#include "scn_nebulae2.h"
#include "scn_rock.h"
#include "scn_test.h"


DemoPtr demo;
Audio g_audio;
ScnNightSkyPtr scn_nightsky;
ScnNebulae1Ptr scn_nebulae1;
ScnNebulae2Ptr scn_nebulae2;
ScnRockPtr         scn_rock;
ScnTestPtr         scn_test;



#include <Atmosphere/Atmosphere.h>

base::GeometryPtr    testgeo;
base::ShaderPtr   testshader;
base::Texture2dPtr testTexture;
AtmosphereGLPtr        atmosphere;
base::ShaderPtr   atmosphereShader;

// ================

#include <PostProcess/PostProcess.ui.h>
#include <Stars/Stars.ui.h>
#include <Atmosphere/Atmosphere.ui.h>
#include <Nebulae/Nebulae.ui.h>

#ifdef STANDALONE
base::GLViewer *glviewer;
base::StopWatch timer;
#else
composer::widgets::GLViewer   *glviewer;
#endif
composer::widgets::CategoryList *editor;


*/


void render( base::Context::Ptr context, base::Camera::Ptr cam )
{
	//glFinish();
	//timer.reset();
	//timer.start();



	g_post->begin();
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glEnable(GL_DEPTH_TEST);
	//context->render(g_geo, g_shader);
	g_volume->render(context, cam);
	g_post->end(context);
	//g_post->render(context);
	//context->renderScreen( g_volume->estimate );


	//context->renderScreen(context->getTexture2d("c:\\projects\\demo\\git\\bin\\data\\uvref.png"));
	//context->renderScreen(context->getTexture2d("target"));

#ifdef STANDALONE
	//std::cout << timer.elapsedSeconds() << std::endl;
	//context->setTime( timer.elapsedSeconds()*24.0f );
#endif
/*
	glFinish();
	timer.stop();
	float elapsed = timer.elapsedSeconds();
	timerMax = std::max(timerMax, elapsed);
	timerMin = std::min(timerMin, elapsed);
	timerAvg += elapsed;
	++numFrames;
*/
}





void init( base::Context::Ptr context )
{
	std::cout << "init...\n";

	g_geo = base::Geometry::createGrid(10, 10, base::Geometry::LINE);
	g_geo->transform( math::M44f::ScaleMatrix(16.0f, 1.0f, 16.0f) );
///*
	// load houdini file ================
	//std::string filename = "c:\\projects\\demo\\git\\bin\\data\\test.bgeo";
	std::string filename = "c:\\projects\\demo\\git\\bin\\data\\manix_bound.bgeo";
	std::ifstream in( filename.c_str(), std::ios_base::in | std::ios_base::binary );
	houdini::HouGeo::Ptr hgeo = houdini::HouGeoIO::import( &in );
	if( hgeo )
	{
		int primIndex = 0;
		houdini::HouGeo::Primitive::Ptr prim = hgeo->getPrimitive(primIndex);

		//geo
		//if(std::dynamic_pointer_cast<houdini::HouGeo::HouPoly>(prim) )
		//	g_geo = houdini::HouGeoIO::convertToGeometry(hgeo, primIndex);
	}

	//g_shader = base::Shader::load( "c:\\projects\\demo\\git\\src\\core\\glsl\\genericShader" );
	//g_shader->setUniform("l", math::V3f(1.0f).normalized());
	//g_shader->setUniform("ka", 0.2f);
	//g_shader->setUniform("ambient", math::V3f(1.0f));
	//g_shader->setUniform("kd", 1.0f);
	//g_shader->setUniform("diffuse", math::V3f(.5f));
	//g_shader = base::Shader::load( "c:\\projects\\demo\\git\\src\\core\\glsl\\simpleColor" );
	//g_shader = base::Shader::load( "c:\\projects\\demo\\git\\src\\core\\glsl\\simpleTexture" );
	//g_shader->setUniform("texture", base::Context::getCurrentContext()->getTexture2d("c:\\projects\\demo\\git\\bin\\data\\uvref.png")->getUniform());

	//base::Texture2d::Ptr target = base::Texture2d::createRGBAFloat32(512, 512);
	//context->addTexture2d("target", target);
	//g_fbo = base::FBO::create().width(512).height(512).attach(target);
//*/
	g_shader = base::Shader::loadFromFile( "c:\\projects\\demo\\git\\src\\core\\glsl\\genericShader" );

	g_volume = Volume::create();
	//g_volume->load( "c:\\projects\\demo\\git\\bin\\data\\manix2.bgeo" );
	g_volume->load( "c:\\projects\\demo\\git\\bin\\data\\artifix_resized.bgeo" );

	// temp for conversion
	//houdini::HouGeoIO::xport("c:\\projects\\demo\\git\\bin\\data\\artifix.bgeo", base::ScalarField::load("c:\\projects\\demo\\temp\\ConvertFile-build\\Debug\\artifix_small.field"));

	// init post process -----------
	g_post = PostProcess::create();
	g_post->setHDREnabled(true);
	//g_post->setGlareEnabled(true);
	//g_post->setGlareBlurIterations(2);
	//g_post->setGlareAmount(0.5f);
	//g_post->setInput(g_volume->estimate);
}


void shutdown()
{
	std::cout << "shutdown...\n";
#ifdef STANDALONE
	//timer.stop();
	//g_audio.stopAudio();
#endif
	//
	// put your deinitialization stuff here

	// stats:
	std::cout << "timer max: " << 1.0/timerMax << "fps / " << timerMax << "s" << std::endl;
	std::cout << "timer min: " << 1.0/timerMin << "fps / " << timerMin << "s" << std::endl;
	timerAvg = timerAvg/float(numFrames);
	std::cout << "timer avg: " << 1.0/timerAvg << "fps / " << timerAvg << "s" << std::endl;
}

/*
void onMouseMove( base::MouseState state )
{
	if( base::Application::getKeyState( KEY_LCONTROL ) )
	{
		float time = (float(state.x) / float(glviewer->width()))*demo->getDuration();
		base::Context::current()->setTime( time );
#ifndef STANDALONE
		glviewer->update();
#endif
	}
}
*/



int main(int argc, char ** argv)
{
	int xres = 512;
	int yres = 512;

#ifdef STANDALONE

	base::Application app;
	glviewer = new base::GLViewer( xres, yres, "app", init, render, shutdown );
	glviewer->getOrbitNavigator().m_distance = 20.0f;
	glviewer->getOrbitNavigator().m_elevation = 45.0f;
	glviewer->getOrbitNavigator().update();
	//glviewer->setMouseMoveCallback( onMouseMove );
	glviewer->show();
	return app.exec();
#else
	/*
	//Q_INIT_RESOURCE(application);
	QApplication app(argc, argv);
	app.setOrganizationName("app");
	app.setApplicationName("app");

	QMainWindow mainWin;
	mainWin.resize(800, 600);
	glviewer = new composer::widgets::GLViewer(init, shutdown, render);
	glviewer->getCamera()->m_znear = .1f;
	glviewer->getCamera()->m_zfar = 1000.0f;
	glviewer->getOrbitNavigator().m_distance = 1.0f;
	glviewer->getOrbitNavigator().m_elevation = 45.0f;
	glviewer->getOrbitNavigator().update();
	glviewer->setMouseMoveCallback( onMouseMove );
	mainWin.setCentralWidget( glviewer );
	mainWin.show();

	return app.exec();
	*/
#endif
}
