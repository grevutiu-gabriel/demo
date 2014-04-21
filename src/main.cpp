//============================================================================
//
//
//============================================================================

#define STANDALONE 1
#define DOAUDIO true

#ifdef STANDALONE
#include <ui/Application.h>
#include <ui/GLViewer.h>
#include <ui/VRWindow.h>
#else
#include <gltools/gl.h>
#include <QtGui>
#include <QApplication>
#include <QMainWindow>
#include "gui/widgets/GLViewer/GLViewer.h"
#include "gui/widgets/TFEditor/TFEWidget.h"
#endif






#include <gfx/Geometry.h>
#include <gfx/Shader.h>
#include <gfx/Texture.h>
#include <gfx/FBO.h>
#include <util/StopWatch.h>


#include <framework/houdini/HouGeoIO.h>
#include <framework/elements/volumept/Volume.h>
#include <framework/elements/postprocess/PostProcess.h>
#include <framework/Property.h>
#include <framework/Demo.h>








#ifdef STANDALONE
base::GLViewer* glviewer;

#else
gui::widgets::GLViewer* glviewer;
#endif



Demo::Ptr g_demo;
base::Geometry::Ptr g_geo;
base::Shader::Ptr g_shader;
base::FBO::Ptr g_fbo;


base::StopWatch g_timer;     // used to drive demo
base::StopWatch g_performanceTimer; // used to measure fps
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
	glFinish();
	g_performanceTimer.reset();
	g_performanceTimer.start();

	// render demo
	glEnable(GL_DEPTH_TEST);
	//g_demo->render( context, g_timer.elapsedSeconds(), cam );
	g_demo->render( context, g_timer.elapsedSeconds() );

	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//context->setView( cam->m_worldToView, cam->m_viewToWorld, cam->m_viewToNDC );



	glFinish();
	g_performanceTimer.stop();
	float elapsed = g_performanceTimer.elapsedSeconds();
	timerMax = std::max(timerMax, elapsed);
	timerMin = std::min(timerMin, elapsed);
	timerAvg += elapsed;
	++numFrames;

}





void init( base::Context::Ptr context )
{
	std::cout << "init...\n";


	// intialize and load demo --------
#ifdef DOAUDIO
	g_demo = Demo::create(true);
#else
	g_demo = Demo::create();
#endif
	g_demo->load("filename");



	// now start demo ------------
#ifdef DOAUDIO
	g_demo->getAudio()->startPlayback();
#endif
	g_timer.start();

}


void shutdown()
{
	std::cout << "shutdown...\n";
#ifdef STANDALONE

	if(g_timer.isRunning())
		g_timer.stop();
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
	///*
	int xres = 800;
	int yres = 600;

#ifdef STANDALONE

	///*
	base::Application app;
	glviewer = new base::GLViewer( xres, yres, "app", init, render, shutdown );
	glviewer->getOrbitNavigator().m_distance = 20.0f;
	glviewer->getOrbitNavigator().m_elevation = 45.0f;
	glviewer->getOrbitNavigator().update();
	//glviewer->setMouseMoveCallback( onMouseMove );
	glviewer->show();
	glviewer->setFullscreen(true);
	return app.exec();
	//*/

/*
	base::Application app;
//	glviewer = new base::GLViewer( xres, yres, "app", init, render, shutdown );
//	glviewer->getOrbitNavigator().m_distance = 20.0f;
//	glviewer->getOrbitNavigator().m_elevation = 45.0f;
//	glviewer->getOrbitNavigator().update();
//	//glviewer->setMouseMoveCallback( onMouseMove );
//	glviewer->show();
	base::VRWindow window;
	window.show();
	return app.exec();
*/
#else
	//Q_INIT_RESOURCE(application);
	QApplication app(argc, argv);
	app.setOrganizationName("app");
	app.setApplicationName("app");

	QMainWindow mainWin;
	mainWin.resize(800, 600);

	glviewer = new gui::widgets::GLViewer(init, shutdown, render);
	/*
	glviewer->getCamera()->m_znear = .1f;
	glviewer->getCamera()->m_zfar = 1000.0f;
	glviewer->getOrbitNavigator().m_distance = 1.0f;
	glviewer->getOrbitNavigator().m_elevation = 45.0f;
	glviewer->getOrbitNavigator().update();
	glviewer->setMouseMoveCallback( onMouseMove );
	*/
	mainWin.setCentralWidget( glviewer );
	mainWin.show();


	Volume::Ptr volume = std::dynamic_pointer_cast<Volume>(g_demo->getShot(0)->getShotElement(0)->getChild(1)->getElement());
	gui::VolumeWrapper::Ptr volumeWrapper = std::make_shared<gui::VolumeWrapper>( volume );
	gui::TFEWidget tfe(volumeWrapper);
	tfe.show();
	volumeWrapper->connect( volumeWrapper.get(), SIGNAL(changed()), glviewer, SLOT(updateGL()) );
	return app.exec();
#endif




}
