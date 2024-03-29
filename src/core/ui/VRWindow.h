#pragma once

#include "Application.h"
#include "Window.h"

#include <gfx/Context.h>
#include <gfx/Camera.h>


//#include <OVR.h>

#ifdef _WINDOWS

namespace base
{

	class VRWindow : public Window
	{
	public:
		typedef void (*InitCallback)( Context::Ptr );
		typedef void (*ShutdownCallback)( void );
		typedef void (*RenderCallback)( Context::Ptr, Camera::Ptr );

		VRWindow( InitCallback init = 0, RenderCallback render = 0, ShutdownCallback shutdown = 0 );                                                           // when using this constructor the window will be created when show is called first time
		VRWindow( int width, int height, std::string caption, InitCallback init = 0, ShutdownCallback shutdown = 0 );
		virtual ~VRWindow();

		virtual void                                                  show(); // shows the window (creates the window when shown the first time)
		virtual void                                                 paint();
		virtual void                                               paintGL();
		virtual void                       setCaption( std::string caption );
		virtual void                        setSize( int width, int height );
		virtual void                                               destroy(); // closes and destroys the window
		void                                setSampleBuffers( bool enabled ); // specifies whether a glcontext with samplebuffer support is to be created
		void                                setStencilBuffer( bool enabled ); // specifies whether a glcontext with stencil support is to be created
		void                                    setSamples( int numSamples ); // specifies the number of samples we want
		void                            setInitCallback( InitCallback init );
		void                setShutdownCallback( ShutdownCallback shutdown );


	private:
		void                                                  createWindow();
		int    choosePixelFormat( HDC pdc, PIXELFORMATDESCRIPTOR *dummyPfd );
		HDC                                                            m_hdc; // device context
		HGLRC                                                          m_hrc; // render context
		HINSTANCE                                                m_hinstance; // program instance
		WNDCLASS                                                        m_wc; // window class

		unsigned int                                           m_pixelformat;
		unsigned long                                            m_dwExstyle;	
		unsigned long                                              m_dwstyle;

		// format options
		int                                                   m_bpp, m_fsbpp;
		int                                                         m_zdepth;
		bool                                                 m_sampleBuffers;
		int                                                     m_numSamples; // -1 indicates: maximum possible
		bool                                                 m_stencilBuffer; // stencil buffer needed?

		bool                                                    m_fullscreen;
		RECT                                                    m_windowRect;
		RECT                                                m_fullscreenRect;

		DEVMODE	                                                   m_DMsaved; // Saves The Previous Screen Settings (NEW)

		// callbacks
		InitCallback                                                  m_init;
		bool                                                   m_initialized;
		ShutdownCallback                                          m_shutdown;
		RenderCallback                                              m_render;
	};
}
#endif

#ifdef linux

#include <gltools/gl.h>



namespace base
{
	class VRWindow : public Window
	{
	public:
		VRWindow( int width, int height, std::string caption );
		virtual ~VRWindow();

		virtual void paint();
		virtual void paintGL();

	private:
		GLXContext                                       m_hrc; // render context

	};
}

#endif









/*---------------------------------------------------------------------

A very simple class which encapuslates some Win32API calls for window
creation and management.

----------------------------------------------------------------------*/
/*
#pragma once
#include <windows.h>
#include <string>
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>


// add the opengl library
//#pragma message("     _Adding library: opengl32.lib" ) 
//#pragma comment( lib, "opengl32.lib" )

namespace win32
{
	//
	//
	//
	class VRWindow
	{
		HWND                                            mhwnd;
		HINSTANCE                                  mhinstance;
		HGLRC                                            mhRC;
		WNDCLASS                                           wc;

		unsigned int                              pixelformat;
		unsigned long                               dwExstyle;	
		unsigned long                                 dwstyle;

		int                                        bpp, fsbpp;
		int                                            zdepth;

		bool                                       fullscreen;
		std::wstring                                  caption;           // caption text of the window
		RECT                                       WindowRect,
											   FullscreenRect;

		DEVMODE	                                      DMsaved;           // Saves The Previous Screen Settings (NEW)

	public:
		HDC                                              mhDC;

		VRWindow();
		~VRWindow();


		bool            createVRWindow( std::string  _caption,           // title of the window
												   int _width,           // width of the window
												  int _height,           // height of the window
												  int _startx,           // x offset of the window
												  int _starty,           // y offset of the window
													 int _bpp,           // bit depth of the window
											 bool _fullscreen,           // fullscreen ?
									  WNDPROC WindowProcedure,           // window handling procedure
									   HINSTANCE _hInstance );

		void                                     show( void );           // shows the window

		HWND                                      getHandle();
		int                                  getWidth( void );
		int                                 getHeight( void );

		bool              toggleFullscreen( void );
		bool setFullScreen( int width, int height, int _bpp );
		void         restoreScreen( bool show_cursor = true ); 


	private:


		bool            _SetPixelFormat(HWND hWnd);           // set the pixelformat of this window
	};
}
*/
