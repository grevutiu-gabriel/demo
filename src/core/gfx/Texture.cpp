#include "Texture.h"

#include <gltools/gl.h>
#include <gltools/misc.h>

#include <iostream>


namespace base
{

	//
	// Texture1d ---------------------------------
	//


	Texture1dPtr Texture1d::create( int textureFormat, int xres )
	{
		Texture1dPtr result = Texture1dPtr(new Texture1d());

		result->m_xres = xres;
		result->m_textureFormat = textureFormat;

		glBindTexture(GL_TEXTURE_1D, result->m_id);
		glTexImage1D(GL_TEXTURE_1D, 0, result->m_textureFormat, result->m_xres, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

		return result;
	}


	Texture1dPtr Texture1d::createR8( int xres )
	{
		return Texture1d::create( GL_R8, xres );
	}

	Texture1dPtr Texture1d::createRGBA8( int xres )
	{
		return Texture1d::create( GL_RGBA8, xres );
	}

	Texture1dPtr Texture1d::createRGBAFloat32( int xres )
	{
		return Texture1d::create( GL_RGBA32F, xres );
	}

	Texture1dPtr Texture1d::createFloat32( int xres )
	{
		return Texture1d::create( GL_R32F, xres );
	}


	Texture1d::Texture1d()
	{
		glGenTextures(1, &m_id);
		glBindTexture(GL_TEXTURE_1D, m_id);

		// when texture area is small, bilinear filter the closest mipmap
		glTexParameterf( GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		//glTexParameterf( GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		// when texture area is large, bilinear filter the original
		glTexParameterf( GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		//glTexParameterf( GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

		// the texture wraps over at the edges (repeat)
		glTexParameterf( GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	}

	Texture1d::~Texture1d()
	{
		glDeleteTextures(1,&m_id);
	}

	void Texture1d::uploadR8( int xres, const unsigned char *pixels )
	{
		m_xres = xres;
		glBindTexture(GL_TEXTURE_1D, m_id);
		glTexImage1D(GL_TEXTURE_1D, 0, m_textureFormat, m_xres, 0, GL_RED, GL_UNSIGNED_BYTE, pixels);
	}

	void Texture1d::uploadRGBA8( int xres, unsigned char *pixels )
	{
		m_xres = xres;
		glBindTexture(GL_TEXTURE_1D, m_id);
		glTexImage1D(GL_TEXTURE_1D, 0, m_textureFormat, m_xres, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	}

	void Texture1d::uploadRGBAFloat32( int xres, float *pixels )
	{
		m_xres = xres;
		glBindTexture(GL_TEXTURE_1D, m_id);
		glTexImage1D(GL_TEXTURE_1D, 0, m_textureFormat, m_xres, 0, GL_RGBA, GL_FLOAT, pixels);
	}

	void Texture1d::uploadFloat32( int xres, float *pixels )
	{
		m_xres = xres;
		glBindTexture(GL_TEXTURE_1D, m_id);
		glTexImage1D(GL_TEXTURE_1D, 0, m_textureFormat, m_xres, 0, GL_RED, GL_FLOAT, pixels);
	}

	AttributePtr Texture1d::getUniform()
	{
		if( !m_uniform )
		{
			m_uniform = Attribute::createSampler1d();
			m_uniform->appendElement( (int)m_id );
		}
		return m_uniform;
	}






	//
	// Texture2d ---------------------------------
	//






	Texture2dPtr Texture2d::create( int textureFormat, int xres, int yres, bool multisampled, int numSamples )
	{
		Texture2dPtr result = Texture2dPtr(new Texture2d(multisampled, numSamples));

		result->m_xres = xres;
		result->m_yres = yres;
		result->m_textureFormat = textureFormat;
		glBindTexture(result->m_target, result->m_id);

		if( !multisampled )
			glTexImage2D(GL_TEXTURE_2D, 0, result->m_textureFormat, result->m_xres, result->m_yres, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		else
			glTexImage2DMultisample( GL_TEXTURE_2D_MULTISAMPLE, numSamples, result->m_textureFormat, result->m_xres, result->m_yres, true );

		if( CHECKGLERROR() )
		{

			std::cout << "Texture2d::create: unable to allocate gpu memory for texture\n";
			return Texture2dPtr();
			//TODO: investigate exception handling + qt throw std::runtime_error(glErrorString(error).c_str());
		}

		return result;
	}

	int Texture2d::width()
	{
		return m_xres;
	}


	int Texture2d::height()
	{
		return m_yres;
	}


	Texture2dPtr Texture2d::createRGBA8( int xres, int yres )
	{
		return Texture2d::create( GL_RGBA8, xres, yres );
	}

	Texture2dPtr createSRGB8( int xres = 64, int yres = 64 )
	{
		return Texture2d::create( GL_SRGB8, xres, yres );
	}

	Texture2dPtr Texture2d::createRGBAFloat32( int xres, int yres, bool multisampled, int numSamples )
	{
		//return Texture2d::create( GL_RGBA_FLOAT32_ATI, xres, yres, multisampled, numSamples );
		return Texture2d::create( GL_RGBA32F, xres, yres, multisampled, numSamples );
	}

	Texture2dPtr Texture2d::createRGBAFloat16( int xres, int yres )
	{
		return Texture2d::create( GL_RGBA16F_ARB, xres, yres );
	}

	Texture2dPtr Texture2d::createRGBFloat32( int xres, int yres )
	{
		return Texture2d::create( GL_RGB32F_ARB, xres, yres );
	}

	Texture2dPtr Texture2d::createFloat32( int xres, int yres )
	{
		return Texture2d::create( GL_R32F, xres, yres);
	}

	Texture2dPtr Texture2d::createFloat16( int xres, int yres )
	{
		return Texture2d::create( GL_R16F, xres, yres);
	}

	// loads texture from file
	// assumes file is gamma corrected (~SRGB color space)
	Texture2dPtr Texture2d::load( const Path &file, int format )
	{
		ImagePtr img = Image::load( file );
		if(img)
		{
			// take opengl uv space into account (bottom up)
			img->flip( true, false );

			//TODO: take image format into account and apply policy. atm rgba8 is assumed
			Texture2dPtr txt = Texture2d::create(format);
			txt->upload( img );
			return txt;
		}
		return Texture2d::Ptr();
	}

	// reference  texture
	Texture2dPtr Texture2d::createUVRefTexture()
	{
		return load( base::path( "base" ) + "/data/textures/uvref.png" );
	}


	Texture2d::Texture2d(bool multisampled, int numSamples) : m_multiSampled(multisampled), m_numSamples(numSamples)
	{
		if(m_multiSampled)
		{
			m_target = GL_TEXTURE_2D_MULTISAMPLE;
		}
		else
			m_target = GL_TEXTURE_2D;


		glGenTextures(1, &m_id);
		glBindTexture(m_target, m_id);

		// multisampled textures only support nearest neighbour lookup
		if( !m_multiSampled )
		{
			//glTexParameteri( m_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
			glTexParameteri( m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			//glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
			// when texture area is large, bilinear filter the original
			//glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
			glTexParameteri( m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			//glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );

			// the texture wraps over at the edges (repeat)
			glTexParameterf( m_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
			glTexParameterf( m_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		}

		CHECKGLERROR();


	}

	Texture2d::~Texture2d()
	{
		glDeleteTextures(1,&m_id);
	}

	void Texture2d::resize( int newWidth, int newHeight )
	{
		if( (m_xres == newWidth)&&(m_yres == newHeight) )
			// nothing to do here
			return;
		m_xres = newWidth;
		m_yres = newHeight;

		glBindTexture(m_target, m_id);
		if( !m_multiSampled )
			// this will destroy whatever we have on the gpu
			glTexImage2D(GL_TEXTURE_2D, 0, m_textureFormat, m_xres, m_yres, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		else
			glTexImage2DMultisample( GL_TEXTURE_2D_MULTISAMPLE, m_numSamples, m_textureFormat, m_xres, m_yres, true );

	}

	void Texture2d::uploadRGBA8( int xres, int yres, unsigned char *pixels )
	{
		m_xres = xres;
		m_yres = yres;
		glBindTexture(m_target, m_id);
		if( !m_multiSampled )
			glTexImage2D(m_target, 0, m_textureFormat, m_xres, m_yres, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	}

	void Texture2d::uploadRGBAFloat32( int xres, int yres, float *pixels )
	{
		m_xres = xres;
		m_yres = yres;
		glBindTexture(m_target, m_id);
		if( !m_multiSampled )
			glTexImage2D(m_target, 0, m_textureFormat, m_xres, m_yres, 0, GL_RGBA, GL_FLOAT, pixels);
	}

	void Texture2d::uploadRGBFloat32( int xres, int yres, float *pixels )
	{
		m_xres = xres;
		m_yres = yres;
		glBindTexture(m_target, m_id);
		if( !m_multiSampled )
			glTexImage2D(m_target, 0, m_textureFormat, m_xres, m_yres, 0, GL_RGB, GL_FLOAT, pixels);
	}

	void Texture2d::uploadFloat32( int xres, int yres, float *pixels )
	{
		m_xres = xres;
		m_yres = yres;
		glBindTexture(m_target, m_id);
		if( !m_multiSampled )
			glTexImage2D(m_target, 0, m_textureFormat, m_xres, m_yres, 0, GL_RED, GL_FLOAT, pixels);
	}

	void Texture2d::upload( ImagePtr image )
	{
		m_xres = image->m_width;
		m_yres = image->m_height;
		glBindTexture(m_target, m_id);

		glTexParameteri( m_target, GL_GENERATE_MIPMAP, GL_TRUE);
		glTexParameteri( m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		glTexParameteri( m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

		if( !m_multiSampled )
			glTexImage2D(m_target, 0, m_textureFormat, m_xres, m_yres, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->m_data);
	}

	ImagePtr Texture2d::download( int format, int componentType )
	{
		ImagePtr img = ImagePtr( new Image() );
		// note that we allocate sizeof(float)
		//TODO: manage pixelformat in Image
		img->m_data = (unsigned char *)malloc( m_xres*m_yres*4*sizeof(float) );
		img->m_width = m_xres;
		img->m_height = m_yres;

		glBindTexture(m_target, m_id);
		glGetTexImage( GL_TEXTURE_2D, 0, format, componentType, img->m_data );

		return img;
	}


	AttributePtr Texture2d::getUniform()
	{
		if( !m_uniform )
		{
			if( !m_multiSampled )
				m_uniform = Attribute::createSampler2d();
			else
				m_uniform = Attribute::createSampler2dMS();
			m_uniform->appendElement( (int)m_id );
		}
		return m_uniform;
	}






	//
	// Texture3d ---------------------------------
	//



	Texture3dPtr Texture3d::create( int textureFormat, int xres, int yres, int zres )
	{
		Texture3dPtr result = Texture3dPtr(new Texture3d());

		result->m_xres = xres;
		result->m_yres = yres;
		result->m_zres = zres;
		result->m_textureFormat = textureFormat;

		glBindTexture(GL_TEXTURE_3D, result->m_id);
		glBindBuffer( GL_PIXEL_UNPACK_BUFFER, 0 );
		glTexImage3D( GL_TEXTURE_3D, 0, result->m_textureFormat, result->m_xres, result->m_yres, result->m_zres, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		CHECKGLERROR();

		return result;
	}

	Texture3dPtr Texture3d::createRGBA8( int xres, int yres, int zres )
	{
		return Texture3d::create( GL_RGBA8, xres, yres, zres );
	}

	Texture3dPtr Texture3d::createRGBAFloat16( int xres, int yres, int zres )
	{
		return Texture3d::create( GL_RGBA16F_ARB, xres, yres, zres );
	}

	Texture3dPtr Texture3d::createRGBAFloat32( int xres, int yres, int zres )
	{
		return Texture3d::create( GL_RGBA32F, xres, yres, zres );
	}

	Texture3dPtr Texture3d::createFloat32( int xres, int yres, int zres )
	{
		return Texture3d::create( GL_R32F, xres, yres, zres );
	}

	Texture3dPtr Texture3d::createFloat16( int xres, int yres, int zres )
	{
		return Texture3d::create( GL_ALPHA16F_ARB, xres, yres, zres );
	}


	Texture3d::Texture3d()
	{
		glGenTextures(1, &m_id);
		glBindTexture(GL_TEXTURE_3D, m_id);



		// when texture area is small, bilinear filter the closest mipmap
		//glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		// when texture area is large, bilinear filter the original
		//glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

		// the texture wraps over at the edges (repeat)
		glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
	}

	Texture3d::~Texture3d()
	{
		glDeleteTextures(1,&m_id);
	}


	void Texture3d::uploadRGBA8( int xres, int yres, int zres, unsigned char *pixels )
	{
		m_xres = xres;
		m_yres = yres;
		m_zres = zres;
		glBindTexture(GL_TEXTURE_3D, m_id);
		glTexImage3DEXT(GL_TEXTURE_3D, 0, m_textureFormat, m_xres, m_yres, m_zres, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	}

	void Texture3d::uploadRGBAFloat32( int xres, int yres, int zres, float *pixels )
	{
		m_xres = xres;
		m_yres = yres;
		m_zres = zres;
		glBindTexture(GL_TEXTURE_3D, m_id);
		glTexImage3D(GL_TEXTURE_3D, 0, m_textureFormat, m_xres, m_yres, m_zres, 0, GL_RGBA, GL_FLOAT, pixels);
	}

	void Texture3d::uploadFloat32( int xres, int yres, int zres, float *pixels )
	{
		m_xres = xres;
		m_yres = yres;
		m_zres = zres;
		glBindTexture(GL_TEXTURE_3D, m_id);
		glTexImage3D(GL_TEXTURE_3D, 0, m_textureFormat, m_xres, m_yres, m_zres, 0, GL_RED, GL_FLOAT, pixels);
	}

	// very premature way to get the the texture data onto cpu
	// TODO: use voxelgrid
	float *Texture3d::download()
	{
		// note that we allocate sizeof(float)
		//TODO: manage pixelformat in Image
		float *m_data = (float *)malloc( m_xres*m_yres*m_zres*4*sizeof(float) );

		glBindTexture(GL_TEXTURE_3D, m_id);
		glGetTexImage( GL_TEXTURE_3D, 0, GL_RGBA, GL_FLOAT, m_data );

		return m_data;
	}

	AttributePtr Texture3d::getUniform()
	{
		if( !m_uniform )
		{
			m_uniform = Attribute::createSampler3d();
			m_uniform->appendElement( (int)m_id );
		}
		return m_uniform;
	}












	//
	// Texture2dArray ---------------------------------
	//



	Texture2dArrayPtr Texture2dArray::create( int textureFormat, int xres, int yres, int zres )
	{
		Texture2dArrayPtr result = Texture2dArrayPtr(new Texture2dArray());

		result->m_xres = xres;
		result->m_yres = yres;
		result->m_zres = zres;
		result->m_textureFormat = textureFormat;

		glBindTexture(GL_TEXTURE_2D_ARRAY, result->m_id);
		glTexImage3D(GL_TEXTURE_3D, 0, result->m_textureFormat, result->m_xres, result->m_yres, result->m_zres, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

		return result;
	}

	Texture2dArrayPtr Texture2dArray::createRGBA8( int xres, int yres, int zres )
	{
		return Texture2dArray::create( GL_RGBA8, xres, yres, zres );
	}

	Texture2dArrayPtr Texture2dArray::createRGBAFloat32( int xres, int yres, int zres )
	{
		return Texture2dArray::create( GL_RGBA_FLOAT32_ATI, xres, yres, zres );
	}

	Texture2dArrayPtr Texture2dArray::createRGBAFloat16( int xres, int yres, int zres )
	{
		return Texture2dArray::create( GL_RGBA16F_ARB, xres, yres, zres );
	}

	Texture2dArrayPtr Texture2dArray::createFloat32( int xres, int yres, int zres )
	{
		return Texture2dArray::create( GL_ALPHA, xres, yres, zres );
	}

	Texture2dArrayPtr Texture2dArray::createDepth24( int xres, int yres, int zres )
	{
		return Texture2dArray::create( GL_DEPTH_COMPONENT24, xres, yres, zres );
	}


	Texture2dArray::Texture2dArray()
	{
		glGenTextures(1, &m_id);
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_id);

		// when texture area is small, bilinear filter the closest mipmap
		//glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameterf( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		// when texture area is large, bilinear filter the original
		//glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameterf( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

		// the texture wraps over at the edges (repeat)
		glTexParameterf( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameterf( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		//glTexParameterf( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
	}

	Texture2dArray::~Texture2dArray()
	{
		glDeleteTextures(1,&m_id);
	}


	void Texture2dArray::uploadRGBA8( int xres, int yres, int zres, unsigned char *pixels )
	{
		m_xres = xres;
		m_yres = yres;
		m_zres = zres;
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_id);
		glTexImage3DEXT(GL_TEXTURE_2D_ARRAY, 0, m_textureFormat, m_xres, m_yres, m_zres, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	}

	void Texture2dArray::uploadRGBAFloat32( int xres, int yres, int zres, float *pixels )
	{
		m_xres = xres;
		m_yres = yres;
		m_zres = zres;
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_id);
		glTexImage3DEXT(GL_TEXTURE_2D_ARRAY, 0, m_textureFormat, m_xres, m_yres, m_zres, 0, GL_RGBA, GL_FLOAT, pixels);
	}

	void Texture2dArray::uploadFloat32( int xres, int yres, int zres, float *pixels )
	{
		m_xres = xres;
		m_yres = yres;
		m_zres = zres;
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_id);
		glTexImage3DEXT(GL_TEXTURE_2D_ARRAY, 0, m_textureFormat, m_xres, m_yres, m_zres, 0, GL_ALPHA, GL_FLOAT, pixels);
	}

	void Texture2dArray::upload( int xres, int yres, int zres, int format, int component, void *data )
	{
		m_xres = xres;
		m_yres = yres;
		m_zres = zres;
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_id);
		glTexImage3DEXT(GL_TEXTURE_2D_ARRAY, 0, m_textureFormat, m_xres, m_yres, m_zres, 0, format, component, data);
	}

	AttributePtr Texture2dArray::getUniform()
	{
		if( !m_uniform )
		{
			m_uniform = Attribute::createSampler2dArray();
			m_uniform->appendElement( (int)m_id );
		}
		return m_uniform;
	}

	//
	// TextureCube ---------------------------------
	//

	TextureCubePtr TextureCube::createRGBA8( int xres, int yres )
	{
		TextureCubePtr result = TextureCubePtr(new TextureCube());

		result->m_textureFormat = GL_RGBA8;

		return result;
	}

	TextureCubePtr TextureCube::createSRGBA8(int xres, int yres)
	{
		TextureCubePtr result = TextureCubePtr(new TextureCube());

		result->m_textureFormat = GL_SRGB8;

		return result;
	}

	TextureCube::TextureCube()
	{

		glGenTextures(1, &m_id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 

	}

	TextureCube::~TextureCube()
	{
		glDeleteTextures(1,&m_id);
	}






	// assumes Image contains all 6 faces
	void TextureCube::upload( ImagePtr image )
	{

		int width = image->m_width/3;
		int height = image->m_height/4;

		ImagePtr faces[6];
		faces[0] = image->copy( width*2, height*1, width, height );
		faces[1] = image->copy( width*0, height*1, width, height );
		faces[2] = image->copy( width*1, height*0, width, height );
		faces[3] = image->copy( width*1, height*2, width, height );
		faces[4] = image->copy( width*1, height*1, width, height );
		faces[5] = image->copy( width*1, height*3, width, height );
		faces[5]->flip();

		glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, m_textureFormat, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, faces[0]->m_data);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, m_textureFormat, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, faces[1]->m_data);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, m_textureFormat, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, faces[2]->m_data);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, m_textureFormat, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, faces[3]->m_data);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, m_textureFormat, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, faces[4]->m_data);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, m_textureFormat, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, faces[5]->m_data);

	}

	void TextureCube::upload( ImagePtr faces[6] )
	{
		int width = faces[0]->m_width;
		int height = faces[0]->m_height;
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, m_textureFormat, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, faces[0]->m_data);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, m_textureFormat, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, faces[1]->m_data);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, m_textureFormat, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, faces[2]->m_data);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, m_textureFormat, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, faces[3]->m_data);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, m_textureFormat, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, faces[4]->m_data);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, m_textureFormat, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, faces[5]->m_data);
	}

	AttributePtr TextureCube::getUniform()
	{
		if( !m_uniform )
		{
			m_uniform = Attribute::createSamplerCube();
			m_uniform->appendElement( (int)m_id );
		}
		return m_uniform;
	}

}
