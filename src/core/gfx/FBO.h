//
// http://www.opengl.org/wiki/Framebuffer_Objects
//

#pragma once
#include <util/shared_ptr.h>
#include <gltools/gl.h>
#include "Texture.h"

namespace base
{

	BASE_DECL_SMARTPTR_STRUCT(FBO)
	struct FBO
	{
		typedef std::shared_ptr<FBO> Ptr;

		struct FBOSetup
		{
			FBOSetup();

			int                                      getWidth() const;
			int                                     getHeight() const;
			bool                               isMultisampled() const;
			int                                 getNumSamples() const;
			int                                 getNumOutputs() const;
			bool                             hasStencilBuffer() const;
			bool                               hasDepthBuffer() const;

			FBOSetup                              &width( int width );
			FBOSetup                            &height( int height );
			FBOSetup                     &multisample( bool enabled );
			FBOSetup                    &numSamples( int numSamples );
			FBOSetup                   &attach( Texture2dPtr output );
			FBOSetup                   &attach( Texture3dPtr output );
			FBOSetup             &attach( Texture2dArrayPtr outputs );
			FBOSetup                   &stencilBuffer( bool enabled );
			FBOSetup                     &depthBuffer( bool enabled );

			operator FBOPtr ();

			std::vector<Texture2dPtr>                       m_outputs;
			std::vector<Texture3dPtr>                     m_3doutputs;
			Texture2dArrayPtr                   m_textureArrayOutputs;
			int                                               m_width;
			int                                              m_height;
			bool                                       m_multisampled;
			int                                          m_numSamples;
			bool                                      m_stencilBuffer;
			bool                                        m_depthBuffer;
			float                                     m_clearColor[4];
			int                                        m_clearStencil;
		};

		FBO( const FBOSetup &setup );
		FBO( int _width, int _height, bool multisampled = false, int numSamples = 4 );
		~FBO();

		static FBOSetup                                                         create();
		void begin( unsigned int clearBits = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		void                                                                       end();
		base::Texture2dPtr                             getAttachedTexture2d( int index );
		int                                                                      width();
		int                                                                     height();

		void setOutputs( Texture2dPtr out0, Texture2dPtr out1 = Texture2dPtr(), Texture2dPtr out2 = Texture2dPtr(), Texture2dPtr out3 = Texture2dPtr() );
		void setOutputs( Texture2dArrayPtr out );
		void setSize( int newXres, int newYres );
		void finalize();



		unsigned int fboId;
		unsigned int m_depthbufferId;
		int             m_numOutputs;
		FBOSetup             m_setup;
	};
}
