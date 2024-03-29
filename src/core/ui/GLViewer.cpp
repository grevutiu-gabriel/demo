#include "GLViewer.h"
#include "Application.h"

#include <iostream>
#include <string>

#include <gltools/misc.h>
#include <gltools/gl.h>

namespace base
{
	// when using this constructor the window will be created when show is called first time
	GLViewer::GLViewer(): GLWindow(), m_render(0)
	{
	}

	GLViewer::GLViewer( int width, int height, std::string caption, GLWindow::InitCallback init, RenderCallback render, ShutdownCallback shutdown ) : GLWindow( width, height, caption, init, shutdown ), m_render(render)
	{
		getCamera()->setRaster(width, height, float(width)/float(height));
	}

	GLViewer::~GLViewer()
	{
	}




	void GLViewer::paintGL()
	{
		EventInfo &ie = Application::eventInfo();

		// check if esc has been pressed
		if(ie.keyb.press[KEY_ESCAPE])
			Application::quit();

//		// if a mousebutton had been pressed
//		if( ie.mouse.buttons[0] || ie.mouse.buttons[1] || ie.mouse.buttons[2] )
//		{
//			if( ie.mouse.buttons[0] )
//			{
//				m_orbitNavigator.orbitView( (float)(ie.mouse.dx)*0.5f,(float) (ie.mouse.dy)*0.5f );
//			}else
//			if( ie.mouse.buttons[1] )
//			{
//				// Alt + RMB => move camera along lookat vector
//				m_orbitNavigator.zoomView( -ie.mouse.dx*m_orbitNavigator.getDistance()*0.005f );
//			}else
//			{// MMBUTTON
//				m_orbitNavigator.panView( (float)ie.mouse.dx, (float)-ie.mouse.dy );
//			}

//			//printf( "%f  %f   %f\n", dbgNav.azimuth, dbgNav.elevation, cam->focalLength );
//		}

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

	void GLViewer::mouseMove(MouseState &mouse)
	{
		bool accepted = false;
		if( m_mouseMove )
			accepted = m_mouseMove(mouse);

		if(!accepted)
		{
	//		std::cout << "testo: " << mouse.ox << " " << mouse.oy << std::endl;std::flush(std::cout);
	//		std::cout << "testp: " << mouse.x << " " << mouse.y << std::endl;std::flush(std::cout);
	//		std::cout << "testd: " << mouse.dx << " " << mouse.dy << std::endl;std::flush(std::cout);
			// if a mousebutton had been pressed
			if( mouse.buttons[0] || mouse.buttons[1] || mouse.buttons[2] )
			{
				if( mouse.buttons[0] )
				{
					m_orbitNavigator.orbitView( (float)(mouse.dx)*0.5f,(float) (mouse.dy)*0.5f );
				}else
				if( mouse.buttons[1] )
				{
					// Alt + RMB => move camera along lookat vector
					m_orbitNavigator.zoomView( -mouse.dx*m_orbitNavigator.getDistance()*0.005f );
				}else
				{// MMBUTTON
					m_orbitNavigator.panView( (float)mouse.dx, (float)-mouse.dy );
				}

				//printf( "%f  %f   %f\n", dbgNav.azimuth, dbgNav.elevation, cam->focalLength );
			}
		}
	}

	Camera::Ptr GLViewer::getCamera()
	{
		return m_orbitNavigator.m_camera;
	}


	OrbitNavigator &GLViewer::getOrbitNavigator()
	{
		return m_orbitNavigator;
	}
/*
	void GLViewer::setView( math::Vec3f lookat, float distance, float azimuth, float elevation )
	{
		m_orbitNavigator.m_lookAt = lookat;
		m_orbitNavigator.m_distance = distance;
		m_orbitNavigator.m_azimuth = azimuth;
		m_orbitNavigator.m_elevation = elevation;
		m_orbitNavigator.update();
	}
*/
	void GLViewer::setRenderCallback( RenderCallback render )
	{
		m_render = render;
	}

}


