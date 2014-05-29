#include <gltools/gl.h>
#include <gltools/misc.h>

#include "GLViewer.h"

#include <iostream>
#include <QCoreApplication>



namespace gui
{
	namespace widgets
	{
		GLViewer::GLViewer( InitCallback init, ShutdownCallback shutdown, RenderCallback render, QWidget *parent ) : QGLWidget(parent), m_init(init), m_shutdown(shutdown), m_render(render), m_mouseMove(0), m_keyPress(0), m_lastX(0), m_lastY(0)
		{
			QGLFormat format(QGL::SampleBuffers | QGL::DoubleBuffer | QGL::AlphaChannel);  
			format.setSamples(16);  
			setFormat(format);  
			makeCurrent();

			// ?
			setFocusPolicy( Qt::StrongFocus );
			// ...
			setMouseTracking( true );
			// this will make sure swapbuffers is not called by qt when doing double buffering
			setAutoBufferSwap(false);

			//
			memset( m_keyboardState.press, 0, 256 * sizeof(int) );
			memset( m_keyboardState.state, 0, 256 * sizeof(int) );

			m_qtKey[ Qt::Key_S ] = KEY_S;
			m_qtKey[ Qt::Key_Q ] = KEY_Q;
			m_qtKey[ Qt::Key_W ] = KEY_W;
			m_qtKey[ Qt::Key_E ] = KEY_E;
			m_qtKey[ Qt::Key_R ] = KEY_R;
			m_qtKey[ Qt::Key_T ] = KEY_T;
			m_qtKey[ Qt::Key_G ] = KEY_G;
			m_qtKey[ Qt::Key_Space ] = KEY_SPACE;
			m_qtKey[ Qt::Key_Return ] = KEY_RETURN;
		}


		GLViewer::~GLViewer()
		{
			if( m_shutdown )
				m_shutdown();
		}

		void GLViewer::setMouseMoveCallback( MouseMoveCallback mouseMoveCallback )
		{
			m_mouseMove = mouseMoveCallback;
		}

		void GLViewer::setKeyPressCallback( KeyPressCallback keyPressCallback )
		{
			m_keyPress = keyPressCallback;
		}


		base::Camera::Ptr GLViewer::getCamera()
		{
			return m_orbitNavigator.m_camera;
		}

		void GLViewer::postUpdateEvent()
		{
			QEvent::Type et = (QEvent::Type)(QEvent::User+10);
			QCoreApplication::postEvent( this, new QEvent( static_cast< QEvent::Type >( et ) ), Qt::LowEventPriority );
		}

		base::OrbitNavigator &GLViewer::getOrbitNavigator()
		{
			return m_orbitNavigator;
		}

		void GLViewer::initializeGL()
		{
			GLenum glewResult = glewInit();
			if (GLEW_OK != glewResult)
			{
				std::cout << "glew init failed\n";
			}

			base::Context::Ptr context = std::make_shared<base::Context>();
			base::Context::setCurrentContext(context);

			context->pushViewport( width(), height() );
			if(m_init)
				m_init(context);
		}

		void GLViewer::resizeGL(int w, int h)
		{
			// setup viewport, projection etc.:
			glViewport(0, 0, (GLint)w, (GLint)h);
		}

		void GLViewer::paintGL()
		{
			if(m_render)
				m_render(base::Context::getCurrentContext(), m_orbitNavigator.m_camera);
			swapBuffers();
		}
		void GLViewer::mouseMoveEvent( QMouseEvent * event )
		{
			Qt::MouseButtons buttons = event->buttons();
			int dx = event->x() - m_lastX;
			int dy = event->y() - m_lastY;
			m_lastX = event->x();
			m_lastY = event->y();

			bool accepted = false;
			if(m_mouseMove)
			{
				base::MouseState ms;
				if( buttons & Qt::LeftButton )
					ms.buttons[0] = 1;
				else
					ms.buttons[0] = 0;
				if( buttons & Qt::MidButton )
					ms.buttons[1] = 1;
				else
					ms.buttons[1] = 0;
				if( buttons & Qt::RightButton )
					ms.buttons[2] = 1;
				else
					ms.buttons[2] = 0;
				ms.dx = dx;
				ms.dy = dy;
				ms.x = event->x();
				ms.y = event->y();

				accepted = m_mouseMove(ms);
			}

			if(!accepted)
			{
				// if a mousebutton had been pressed
				if( buttons != Qt::NoButton )
				{

					if( buttons & Qt::LeftButton )
					{
						m_orbitNavigator.orbitView( (float)(dx)*0.5f,(float) (dy)*0.5f );
					}else
					if( buttons & Qt::RightButton )
					{
						// Alt + RMB => move camera along lookat vector
						m_orbitNavigator.zoomView( -dx*m_orbitNavigator.getDistance()*0.005f );
					}else
					if( buttons & Qt::MidButton )
					{// MMBUTTON
						m_orbitNavigator.panView( (float)dx, (float)-dy );
					}

					update();
				}
			}
		}

		void GLViewer::keyPressEvent( QKeyEvent * event )
		{
			m_keyboardState.press[ m_qtKey[event->key()] ] = 1;
			if(m_keyPress)
				m_keyPress(m_qtKey[event->key()]);
			update();
		}
		void GLViewer::keyReleaseEvent( QKeyEvent * event )
		{
			m_keyboardState.press[ m_qtKey[event->key()] ] = 0;
			// TODO: callback/update?
		}

		bool GLViewer::event(QEvent* e)
		{
			bool accepted = QGLWidget::event(e);

			//if(!accepted)
			{
				QEvent::Type et = (QEvent::Type)(QEvent::User+10);
				if( e->type() == et )
				{
					e->accept();
					this->update();
					return true;
				}
			}
			return accepted;
		}

		base::KeyboardState &GLViewer::getKeyboardState()
		{
			return m_keyboardState;
		}



		void GLViewer::setView( math::Vec3f lookat, float distance, float azimuth, float elevation )
		{
			m_orbitNavigator.m_lookAt = lookat;
			m_orbitNavigator.m_distance = distance;
			m_orbitNavigator.m_azimuth = azimuth;
			m_orbitNavigator.m_elevation = elevation;
			m_orbitNavigator.update();

			update();
		}
	}
}
