#include "VRViewer.h"
#include "Application.h"

#include <iostream>
#include <string>

#include <gltools/misc.h>
#include <gltools/gl.h>

namespace base
{
	// when using this constructor the window will be created when show is called first time
	VRViewer::VRViewer(): GLWindow(), m_render(0)
	{
	}

	VRViewer::VRViewer( int width, int height, std::string caption, GLWindow::InitCallback init, RenderCallback render, ShutdownCallback shutdown ) : GLWindow( width, height, caption, init, shutdown ), m_render(render)
	{
		getCamera()->setRaster(width, height, float(width)/float(height));
	}

	VRViewer::~VRViewer()
	{
	}




	void VRViewer::paintGL()
	{
		EventInfo &ie = Application::eventInfo();

		// check if esc has been pressed
		if(ie.keyb.press[KEY_ESCAPE])
			Application::quit();

		// if a mousebutton had been pressed
		if( ie.mouse.buttons[0] || ie.mouse.buttons[1] || ie.mouse.buttons[2] )
		{
			if( ie.mouse.buttons[0] )
			{
				m_orbitNavigator.orbitView( (float)(ie.mouse.dx)*0.5f,(float) (ie.mouse.dy)*0.5f );
			}else
			if( ie.mouse.buttons[1] )
			{
				// Alt + RMB => move camera along lookat vector
				m_orbitNavigator.zoomView( -ie.mouse.dx*m_orbitNavigator.getDistance()*0.005f );
			}else
			{// MMBUTTON
				m_orbitNavigator.panView( (float)ie.mouse.dx, (float)-ie.mouse.dy );
			}

			//printf( "%f  %f   %f\n", dbgNav.azimuth, dbgNav.elevation, cam->focalLength );
		}

		if(m_render)
			m_render( base::Context::getCurrentContext(), m_orbitNavigator.m_camera );
		else
		{
			/*
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glMatrixMode( GL_PROJECTION );
			glPushMatrix();
			glLoadMatrixf( m_orbitNavigator.m_camera->m_projectionMatrix.ma );

			glMatrixMode( GL_MODELVIEW );
			glPushMatrix();
			glLoadMatrixf( (GLfloat *)m_orbitNavigator.m_camera->m_viewMatrix.ma );

			// draw scene
			drawGrid();

			glMatrixMode( GL_PROJECTION );
			glPopMatrix();

			glMatrixMode( GL_MODELVIEW );
			glPopMatrix();
			*/
		}
	}

	Camera::Ptr VRViewer::getCamera()
	{
		return m_orbitNavigator.m_camera;
	}


	OrbitNavigator &VRViewer::getOrbitNavigator()
	{
		return m_orbitNavigator;
	}
/*
	void VRViewer::setView( math::Vec3f lookat, float distance, float azimuth, float elevation )
	{
		m_orbitNavigator.m_lookAt = lookat;
		m_orbitNavigator.m_distance = distance;
		m_orbitNavigator.m_azimuth = azimuth;
		m_orbitNavigator.m_elevation = elevation;
		m_orbitNavigator.update();
	}
*/
	void VRViewer::setRenderCallback( RenderCallback render )
	{
		m_render = render;
	}

}


