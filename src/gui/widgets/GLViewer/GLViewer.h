#pragma once
#include <gfx/Context.h>
#include <QtOpenGL/QGLWidget>
#include <QMouseEvent>

#include <gfx/OrbitNavigator.h>
#include <gfx/Camera.h>
#include <gfx/Context.h>

#include <ui/EventInfo.h>


namespace gui
{
	namespace widgets
	{
		class GLViewer : public QGLWidget
		{
			Q_OBJECT        // must include this if you use Qt signals/slots

		public:
			typedef void (*InitCallback)( base::Context::Ptr );
			typedef void (*RenderCallback)( base::Context::Ptr context, base::Camera::Ptr );
			typedef void (*ShutdownCallback)( void );
			typedef void (*MouseMoveCallback)( base::MouseState state );
			typedef void (*KeyPressCallback)( base::KeyboardState &state );

			GLViewer( InitCallback init = 0, ShutdownCallback shutdown = 0, RenderCallback render = 0, QWidget *parent = 0 );
			virtual                                                                       ~GLViewer();

			base::KeyboardState                                                   &getKeyboardState();
			void                          setMouseMoveCallback( MouseMoveCallback mouseMoveCallback );
			void                             setKeyPressCallback( KeyPressCallback keyPressCallback );

			void        setView( math::Vec3f lookat, float distance, float azimuth, float elevation );
			base::OrbitNavigator                                                 &getOrbitNavigator();
			base::Camera::Ptr                                                             getCamera();

		public:
		protected:

			virtual void                                                       initializeGL()override;
			virtual void                                               resizeGL(int w, int h)override;
			virtual void                                                            paintGL()override;
			virtual void                                mouseMoveEvent( QMouseEvent * event )override;
			virtual void                                   keyPressEvent( QKeyEvent * event )override;
			virtual void                                 keyReleaseEvent( QKeyEvent * event )override;
		private:
			InitCallback                                                                       m_init;
			RenderCallback                                                                   m_render;
			ShutdownCallback                                                               m_shutdown;
			MouseMoveCallback                                                             m_mouseMove;
			KeyPressCallback                                                               m_keyPress;
			base::OrbitNavigator                                                     m_orbitNavigator;
			int                                                                               m_lastX;
			int                                                                               m_lastY;


			base::KeyboardState                                                       m_keyboardState;
			std::map<int, int>                                                                m_qtKey; // maps qt keys to our keys

			friend class GLThread;
		};
	}
}
