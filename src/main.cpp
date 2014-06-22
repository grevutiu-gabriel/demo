//============================================================================
//
//
//============================================================================

//#define STANDALONE 1
//#define DOAUDIO true

#ifdef STANDALONE
#include <ui/Application.h>
#include <ui/GLViewer.h>
#include <ui/VRWindow.h>
#else
#include <ui/Application.h>
#include "gui/Application.h"
#include "gui/widgets/TFEditor/TFEWidget.h"
#include "gui/wrapper/elements/FlareShopWrapper.h"
#endif

#include <QApplication>




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
#endif



Demo::Ptr g_demo;



base::StopWatch g_timer;     // used to drive demo
base::StopWatch g_performanceTimer; // used to measure fps
float timerMax=-std::numeric_limits<float>::infinity();
float timerAvg=0.0f;
float timerMin=std::numeric_limits<float>::infinity();
int numFrames = 0;
int interactionMode = 0;




void render( base::Context::Ptr context, base::Camera::Ptr cam )
{
	glFinish();
	g_performanceTimer.reset();
	g_performanceTimer.start();

	// render demo
	glEnable(GL_DEPTH_TEST);
	if(interactionMode)
		g_demo->render( context, g_timer.elapsedSeconds(), cam );
	else
		g_demo->render( context, g_timer.elapsedSeconds() );


	glFinish();
	g_performanceTimer.stop();
	float elapsed = g_performanceTimer.elapsedSeconds();
	timerMax = std::max(timerMax, elapsed);
	timerMin = std::min(timerMin, elapsed);
	timerAvg += elapsed;
	++numFrames;


	if(g_timer.elapsedSeconds()>g_demo->getDuration())
		g_timer.setElapsed(0.0f);

	gui::Application::getInstance()->getGlViewer()->postUpdateEvent();
}



void init( base::Context::Ptr context )
{
	std::cout << "init...\n";
	//ObjectFactory::print(std::cout);


	// intialize and load demo --------

#ifdef STANDALONE

//#ifdef DOAUDIO
//	g_demo = Demo::create(true);
//#else
	g_demo = Demo::create();
//#endif
	g_demo->load("filename");

#else
	// gui
	gui::DemoWrapper::Ptr demoWrapper = gui::Application::getInstance()->getDemoWrapper();
	//demoWrapper->load("filename");
	g_demo = demoWrapper->getDemo();

	//if( g_demo->getNumCompositions()>0 )
	//	gui::Application::getInstance()->openCompositionEditor( demoWrapper->getCompositionWrapper(0) );
#endif

	// test: serialize demo ----
	//houdini::json::Value value;
	//g_demo->store(value);
	//g_demo->store("test.json");

	{
		//DemoExporter de(&std::cout);
		//de.exportDemo(value);
		//houdini::json::ObjectPtr root = houdini::json::Value::createObject().asObject();
		//houdini::json::ASCIIWriter writer();
	}


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
	g_demo = Demo::Ptr();

	// stats:
	std::cout << "timer max: " << 1.0/timerMax << "fps / " << timerMax << "s" << std::endl;
	std::cout << "timer min: " << 1.0/timerMin << "fps / " << timerMin << "s" << std::endl;
	timerAvg = timerAvg/float(numFrames);
	std::cout << "timer avg: " << 1.0/timerAvg << "fps / " << timerAvg << "s" << std::endl;
}
#ifdef STANDALONE
bool onMouseMove( base::MouseState& state )
{
	if( base::Application::getKeyState( KEY_LCONTROL ) )
	{
		float time = (float(state.x) / float(glviewer->width()))*g_demo->getDuration();
		g_timer.setElapsed(time);
		return true;
	}
	return false;
}
#else
bool onMouseMove( base::MouseState& state )
{
	if( base::Application::getKeyState( KEY_LCONTROL ) )
	{
		float time = (float(state.x) / float(gui::Application::getInstance()->getGlViewer()->width()))*g_demo->getDuration();
		g_timer.setElapsed(time);
		return true;
	}
	return false;
}

#endif

void onKeyPress( int key )
{
	if( key == KEY_SPACE)
	{
		if(g_timer.isRunning())
			g_timer.stop();
		else
			g_timer.start();
	}else
	if( key == KEY_RETURN)
		interactionMode = !interactionMode;
}



int main(int argc, char ** argv)
{
	base::pathRegister( "src", base::Path( SRC_PATH ) );
	base::pathRegister( "data", base::Path( DATA_PATH ) );

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
	glviewer->setKeyPressCallback( onKeyPress );
	glviewer->setMouseMoveCallback( onMouseMove );
	glviewer->show();
	//glviewer->setFullscreen(true);
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
	gui::Application app(argc, argv, init, shutdown, render);
	app.setOrganizationName("app");
	app.setApplicationName("app");

	app.getGlViewer()->setKeyPressCallback( onKeyPress );
	app.getGlViewer()->setMouseMoveCallback(onMouseMove);

	/*
	glviewer->getCamera()->m_znear = .1f;
	glviewer->getCamera()->m_zfar = 1000.0f;
	glviewer->getOrbitNavigator().m_distance = 1.0f;
	glviewer->getOrbitNavigator().m_elevation = 45.0f;
	glviewer->getOrbitNavigator().update();

	*/
//	mainWin.setCentralWidget( glviewer );
//	mainWin.show();


	//gui::FlareShopWrapper::Ptr fsw;
	//Volume::Ptr volume = std::dynamic_pointer_cast<Volume>(g_demo->getComposition(0)->getCompositionElement(0)->getChild(1)->getElement());
	//gui::VolumeWrapper::Ptr volumeWrapper = std::make_shared<gui::VolumeWrapper>( volume );
	//gui::TFEWidget tfe(volumeWrapper);
	//tfe.show();
	//volumeWrapper->connect( volumeWrapper.get(), SIGNAL(changed()), app.getGlViewer(), SLOT(updateGL()) );

//	if( g_demo->getNumCompositions()>0 )
//	{
//		Composition::Ptr composition = g_demo->getComposition(0);
//		int numCompositionElements = composition->getNumCompositionElements();
//		for( int i=0;i<numCompositionElements;++i )
//		{
//			CompositionElement::Ptr compositionElement = composition->getCompositionElement(i);
//			FlareShop::Ptr fs = std::dynamic_pointer_cast<FlareShop>(compositionElement->getElement());
//			if( fs )
//			{
//				fsw = gui::FlareShopWrapper::create(fs);
//				fsw->connect( fsw.get(), SIGNAL(changed()), app.getGlViewer(), SLOT(updateGL()) );
//			}
//		}
//	}


	return app.exec();
#endif




}
