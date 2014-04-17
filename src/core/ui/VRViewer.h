#pragma once

#include "Application.h"
#include "GLWindow.h"



#include <gfx/Camera.h>
#include <gfx/OrbitNavigator.h>

namespace base
{
	class VRViewer : public GLWindow
	{
	public:
		typedef void (*RenderCallback)( Context::Ptr, Camera::Ptr );
		VRViewer();                                                                                // when using this constructor the window will be created when show is called first time
		VRViewer( int width, int height, std::string caption, GLWindow::InitCallback init = 0, RenderCallback render = 0, ShutdownCallback shutdown = 0 );
		virtual ~VRViewer();

		virtual void         paintGL();
		Camera::Ptr          getCamera();
		void                 setView( math::Vec3f lookat, float distance, float azimuth, float elevation );
		void                 setRenderCallback( RenderCallback render );
		OrbitNavigator&      getOrbitNavigator();

	private:
		OrbitNavigator       m_orbitNavigator;
		RenderCallback       m_render;
	};
}
