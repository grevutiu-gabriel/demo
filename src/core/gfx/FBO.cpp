#include "FBO.h"
#include <iostream>
#include <gltools/gl.h>
#include <gfx/Context.h>

namespace base
{

	FBO::FBOSetup::FBOSetup()
	{
		m_width = -1;
		m_height = -1;
		m_multisampled = false;
		m_numSamples = -1;
		m_stencilBuffer = false;
		m_depthBuffer = true;
		m_clearColor[0] = m_clearColor[1] = m_clearColor[2] = 0.0f;
		m_clearColor[3] = 1.0f;
		m_clearStencil = 0;
	}

	int FBO::FBOSetup::getWidth() const{return m_width;}
	int FBO::FBOSetup::getHeight() const{return m_height;}
	bool FBO::FBOSetup::isMultisampled() const{return (m_multisampled != 0);}
	int FBO::FBOSetup::getNumSamples() const{return m_numSamples;}
	int FBO::FBOSetup::getNumOutputs() const
	{
		if(!m_outputs.empty())
			return (int)(m_outputs.size());
		else if(!m_3doutputs.empty())
			return (int)(m_3doutputs.size());
		else if(m_textureArrayOutputs)
			return std::min( 4, m_textureArrayOutputs->m_zres );
		else
			return 0;
	}

	bool FBO::FBOSetup::hasStencilBuffer() const
	{
		return m_stencilBuffer;
	}

	bool FBO::FBOSetup::hasDepthBuffer() const
	{
		return m_depthBuffer;
	}

	FBO::FBOSetup &FBO::FBOSetup::width( int width )
	{
		m_width = width;
		return *this;
	}

	FBO::FBOSetup &FBO::FBOSetup::height( int height )
	{
		m_height = height;
		return *this;
	}

	FBO::FBOSetup &FBO::FBOSetup::multisample( bool enabled )
	{
		m_multisampled = enabled;
		return *this;
	}

	FBO::FBOSetup &FBO::FBOSetup::numSamples( int numSamples )
	{
		m_numSamples = numSamples;
		return *this;
	}

	FBO::FBOSetup &FBO::FBOSetup::attach( Texture2dPtr output )
	{
		m_outputs.push_back( output );
		return *this;
	}

	FBO::FBOSetup &FBO::FBOSetup::attach( Texture3dPtr output )
	{
		m_3doutputs.push_back( output );
		return *this;
	}

	FBO::FBOSetup &FBO::FBOSetup::attach( Texture2dArrayPtr outputs )
	{
		if( outputs->m_zres > 4 )
			std::cout << "FBO::setOutputs( Texture2dArrayPtr out ) warning current only 4 layers supported \n";
		m_textureArrayOutputs = outputs;
		return *this;
	}

	FBO::FBOSetup &FBO::FBOSetup::stencilBuffer( bool enabled )
	{
		m_stencilBuffer = enabled;
		return *this;
	}

	FBO::FBOSetup &FBO::FBOSetup::depthBuffer( bool enabled )
	{
		m_depthBuffer = enabled;
		return *this;
	}
	
	FBO::FBOSetup::operator FBOPtr()
	{
		return FBOPtr( new FBO( *this ) );
	}



	FBO::FBO( const FBOSetup &setup ) : m_setup(setup)
	{
		// if there is no width/height defined, we will try to derive resolution from texture
		if( (m_setup.getWidth() == -1)||(m_setup.getHeight() == -1) )
			if( !m_setup.m_outputs.empty() )
			{
				m_setup.width( m_setup.m_outputs[0]->width() );
				m_setup.height( m_setup.m_outputs[0]->height() );
			}

		// create framebuffer (A framebuffer object is really just a container that attaches textures and renderbuffers to itself to create a complete configuration needed by the renderer.)
		glGenFramebuffersEXT(1, &fboId);
		// make framebuffer active
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);



		// create depth renderbuffer ===========================
		if( setup.hasDepthBuffer() )
		{
			glGenRenderbuffersEXT(1, &m_depthbufferId);
			glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_depthbufferId);

			int depthComponent = GL_DEPTH_COMPONENT;

			// if we also have a stencil buffer request, we will try to combine depth and stencil into one buffer
			// by using a combined depth-stencil component
			if( setup.hasStencilBuffer() )
				depthComponent = GL_DEPTH_STENCIL_EXT;


			if( !m_setup.isMultisampled() )
				glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, depthComponent, m_setup.getWidth(), m_setup.getHeight());
			else
				glRenderbufferStorageMultisample( GL_RENDERBUFFER_EXT, m_setup.getNumSamples(), depthComponent, m_setup.getWidth(), m_setup.getHeight() );

			// attach depth renderbuffer it to fbo
			glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, m_depthbufferId);

			// attach stencil buffer to fbo
			if( setup.hasStencilBuffer() )
				glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, m_depthbufferId);
		}else
		{
			//TODO: handle case where we have stencil but not depth
		}



		// attach outputs since FBOSetup provides us with all attachments =============
		if( !setup.m_outputs.empty() )
		{
			int i = 0;
			for( std::vector<Texture2dPtr>::const_iterator it = setup.m_outputs.begin(); it != setup.m_outputs.end(); ++it, ++i )
			{
				Texture2dPtr out = *it;
				// make sure texture has the same size
				out->resize( m_setup.m_width, m_setup.m_height );
				// attach texture to framebuffer
				glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT+i, out->m_target, out->m_id, 0);
			}
		}else
		if( !setup.m_3doutputs.empty() )
		{
			int i = 0;
			for( std::vector<Texture3dPtr>::const_iterator it = setup.m_3doutputs.begin(); it != setup.m_3doutputs.end(); ++it, ++i )
			{
				Texture3dPtr out = *it;

				glBindTexture( GL_TEXTURE_3D, out->m_id );
				// make sure texture has the same size
				//out->resize( m_setup.m_width, m_setup.m_height );
				// attach texture to framebuffer

				glFramebufferTextureEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT+i, out->m_id, 0);
				//glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT+i, out->m_target, out->m_id, 0);
			}
		}else
		if( setup.m_textureArrayOutputs )
		{
			// make framebuffer active
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);

			glBindTexture( GL_TEXTURE_2D_ARRAY, setup.m_textureArrayOutputs->m_id );

			if( setup.m_textureArrayOutputs->m_zres > 4 )
				std::cout << "FBO::setOutputs( Texture2dArrayPtr out ) warning current only 4 layers supported \n";
			int numLayers = setup.getNumOutputs();
			for( int i = 0; i < numLayers; ++i )
				glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, setup.m_textureArrayOutputs->m_id, 0, i );

			// unbind framebuffer
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		}else
		{
			glDrawBuffer(GL_NONE);
		}

		m_numOutputs = setup.getNumOutputs();

		//  finalize ===============
		finalize();

		// unbind framebuffer
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}


	FBO::FBO( int _width, int _height, bool multisampled, int numSamples )
	{
		m_setup.width(_width);
		m_setup.height(_height);
		m_setup.multisample(multisampled);
		m_setup.numSamples(numSamples);

		// create framebuffer (A framebuffer object is really just a container that attaches textures and renderbuffers to itself to create a complete configuration needed by the renderer.)
		glGenFramebuffersEXT(1, &fboId);
		// make framebuffer active
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);


		// create depth buffer and attach it to FBO ==========================
		// create render buffer for depth (A renderbuffer is a simple 2D graphics image in a specified format. This format usually is defined as color, depth or stencil data.)
		glGenRenderbuffersEXT(1, &m_depthbufferId);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_depthbufferId);


		// initialize renderbuffer so that it can hold depth ( glRenderbufferStorage — establish data storage, format and dimensions of a renderbuffer object's image -> create depthmap buffer)
		if( !m_setup.isMultisampled() )
			glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, m_setup.getWidth(), m_setup.getHeight());
		else
			glRenderbufferStorageMultisample( GL_RENDERBUFFER_EXT, m_setup.getNumSamples(), GL_DEPTH_COMPONENT, m_setup.getWidth(), m_setup.getHeight() );

		// attach depth renderbuffer it to fbo
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, m_depthbufferId);

		// FBO is incomplete at this point because we havent created and attached a colorbuffer. This is done
		// in setOutputs and we use glFramebufferTexture2DEXT
	}

	FBO::~FBO()
	{
		glDeleteFramebuffersEXT(1, &fboId);
		if( m_setup.hasDepthBuffer() )
			glDeleteRenderbuffersEXT(1, &m_depthbufferId);
	}


	FBO::FBOSetup FBO::create()
	{
		return FBOSetup();
	}

	base::Texture2dPtr FBO::getAttachedTexture2d( int index )
	{
		return m_setup.m_outputs[index];
	}

	int FBO::width()
	{
		return m_setup.m_width;
	}

	int FBO::height()
	{
		return m_setup.m_height;
	}


	void FBO::setOutputs( Texture2dPtr out0, Texture2dPtr out1, Texture2dPtr out2, Texture2dPtr out3 )
	{
		// make framebuffer active
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);

		Texture2dPtr out[4] = { out0, out1, out2, out3 };

		m_setup.m_outputs.clear();
		for( int i=0; i<4;++i )
			if( out[i] )
				m_setup.attach( out[i] );

		int i = 0;
		for( std::vector<Texture2dPtr>::const_iterator it = m_setup.m_outputs.begin(); it != m_setup.m_outputs.end(); ++it, ++i )
		{
			Texture2dPtr out = *it;
			// attach texture to framebuffer
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT+i, out->m_target, out->m_id, 0);
		}

		m_numOutputs = m_setup.getNumOutputs();

		// unbind framebuffer
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}

	void FBO::setOutputs( Texture2dArrayPtr out )
	{
		m_setup.attach( out );

		// make framebuffer active
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);

		glBindTexture( GL_TEXTURE_2D_ARRAY, out->m_id );

		int numLayers = m_setup.getNumOutputs();
        for( int i = 0; i < numLayers; ++i )
            glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, out->m_id, 0, i );

		m_numOutputs = m_setup.getNumOutputs();

		// unbind framebuffer
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}


	void FBO::setSize( int newXres, int newYres )
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);

		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_depthbufferId);
		if( !m_setup.isMultisampled() )
			glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, newXres, newYres);
		else
			glRenderbufferStorageMultisample( GL_RENDERBUFFER_EXT, m_setup.getNumSamples(), GL_DEPTH_COMPONENT, newXres, newYres );


		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

		m_setup.width(newXres);
		m_setup.height(newYres);
	}


	void FBO::finalize()
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);

		GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		switch (status)
		{
			case GL_FRAMEBUFFER_COMPLETE_EXT:
				std::cout << "FBO successfull finalized\n";
			break;
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
				std::cout << "FBO error: GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT" << std::endl;
			break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
				std::cout << "FBO error: GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT" << std::endl;
			break;
			case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
				std::cout << "FBO error: GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT" << std::endl;
			break;
			case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
				std::cout << "FBO error: GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT" << std::endl;
			break;
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
				std::cout << "FBO error: GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT" << std::endl;
			break;
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
				std::cout << "FBO error: GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT" << std::endl;
			break;
			case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
				std::cout << "FBO error: GL_FRAMEBUFFER_UNSUPPORTED_EXT" << std::endl;
			break;
			default:
				std::cout << "FBO error: Unknown error" << std::endl;
			break;
		}

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}

	void FBO::begin( unsigned int clearBits )
	{
		// make framebuffer active
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);

		// save current states
		Context::getCurrentContext()->pushViewport(m_setup.getWidth(), m_setup.getHeight(), this);

		// Set the render target
		if( m_numOutputs )
		{
			GLenum buffers[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_COLOR_ATTACHMENT3_EXT };
			glDrawBuffers(m_setup.getNumOutputs(), buffers);
		}else
			glDrawBuffer(GL_NONE);

		if( clearBits )
		{
			glClearColor( m_setup.m_clearColor[0], m_setup.m_clearColor[1], m_setup.m_clearColor[2], m_setup.m_clearColor[3]);
			glClearStencil( m_setup.m_clearStencil );
			glClear( clearBits );
		}
	}

	void FBO::end()
	{
		Context::ViewportState vs = Context::getCurrentContext()->popViewport();

		// unbind framebuffer
		if( vs.fbo )
		{
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, vs.fbo->fboId);
			// Set the render target
			if( m_numOutputs )
			{
				GLenum buffers[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_COLOR_ATTACHMENT3_EXT };
				glDrawBuffers(vs.fbo->m_setup.getNumOutputs(), buffers);
			}else
				glDrawBuffer(GL_NONE);
		}else
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}

}
