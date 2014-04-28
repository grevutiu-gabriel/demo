#pragma once

#include <util/shared_ptr.h>
#include "Attribute.h"
#include "Image.h"

namespace base
{
	BASE_DECL_SMARTPTR_STRUCT(Texture1d);

	struct Texture1d
	{
		typedef std::shared_ptr<Texture1d> Ptr;

		Texture1d();
		~Texture1d();

		void       uploadR8( int xres, const unsigned char *pixels );
		void          uploadRGBA8( int xres, unsigned char *pixels );
		void            uploadRGBAFloat32( int xres, float *pixels );
		void                uploadFloat32( int xres, float *pixels );
		AttributePtr                                    getUniform();


		unsigned int                                            m_id; // opengl handle
		AttributePtr                                       m_uniform; // will be used to attach textures to geometry/or shaders which then will be bound
		int										              m_xres; // texture info just for the record
		int                                          m_textureFormat; // format of texture in gpu memory

		static Texture1dPtr create( int textureFormat, int xres = 64 );
		static Texture1dPtr createR8( int xres = 64 );
		static Texture1dPtr createRGBA8( int xres = 64 );
		static Texture1dPtr createFloat32( int xres = 64 );
		static Texture1dPtr createRGBAFloat32( int xres = 64 );
	};

	BASE_DECL_SMARTPTR_STRUCT(Texture2d);

	struct Texture2d
	{
		typedef std::shared_ptr<Texture2d> Ptr;
		Texture2d( bool multisampled = false, int numSamples = 4 );
		~Texture2d();

		int                                                                width();
		int                                                               height();
		void                                 resize( int newWidth, int newHeight );

		void              uploadRGBA8( int xres, int yres, unsigned char *pixels );
		void                uploadRGBAFloat32( int xres, int yres, float *pixels );
		void                 uploadRGBFloat32( int xres, int yres, float *pixels );
		void                    uploadFloat32( int xres, int yres, float *pixels );
		void                                              upload( ImagePtr image );
		ImagePtr                         download( int format, int componentType ); // e.g. GL_RGBA, GL_FLOAT
		AttributePtr                                                  getUniform();


		unsigned int                                                          m_id; // opengl handle
		AttributePtr                                                     m_uniform; // will be used to attach textures to geometry/or shaders which then will be bound
		int                                                         m_xres, m_yres; // texture info just for the record
		int                                                        m_textureFormat; // format of texture in gpu memory
		int                                                               m_target; // this is either GL_TEXTURE_2D or GL_TEXTURE_2D_MULTISAMPLE
		bool                                                        m_multiSampled;
		int                                                           m_numSamples;

		static Texture2dPtr create( int textureFormat, int xres = 64, int yres = 64, bool multisampled = false, int numSamples = 4 );
		static Texture2dPtr            createRGBA8( int xres = 64, int yres = 64 );
		static Texture2dPtr            createSRGB8( int xres = 64, int yres = 64 );
		static Texture2dPtr          createFloat32( int xres = 64, int yres = 64 );
		static Texture2dPtr          createFloat16( int xres = 64, int yres = 64 );
		static Texture2dPtr createRGBAFloat32( int xres = 64, int yres = 64, bool multisampled = false, int numSamples = 4 );
		static Texture2dPtr      createRGBAFloat16( int xres = 64, int yres = 64 );
		static Texture2dPtr       createRGBFloat32( int xres = 64, int yres = 64 );


		static Texture2dPtr          load( const Path &file, int format = 0x8058 ); // loads texture from file (0x8058=GL_RGBA8)
		static Texture2dPtr                                   createUVRefTexture(); // reference  texture
	};





	BASE_DECL_SMARTPTR_STRUCT(Texture3d);

	struct Texture3d
	{
		typedef std::shared_ptr<Texture3d> Ptr;
		Texture3d();
		~Texture3d();

		void              uploadRGBA8( int xres, int yres, int zres, unsigned char *pixels );
		void                uploadRGBAFloat32( int xres, int yres, int zres, float *pixels );
		void                    uploadFloat32( int xres, int yres, int zres, float *pixels );
		float                                                                    *download();
		AttributePtr                                                            getUniform();


		unsigned int                                                                    m_id; // opengl handle
		AttributePtr                                                               m_uniform; // will be used to attach textures to geometry/or shaders which then will be bound
		int                                                           m_xres, m_yres, m_zres; // texture info just for the record
		int                                                                  m_textureFormat; // format of texture in gpu memory

		static Texture3dPtr create( int textureFormat, int xres = 64, int yres = 64, int zres = 64 );
		static Texture3dPtr createRGBA8( int xres = 64, int yres = 64, int zres = 64 );
		static Texture3dPtr createFloat32( int xres = 64, int yres = 64, int zres = 64 );
		static Texture3dPtr createFloat16( int xres = 64, int yres = 64, int zres = 64 );
		static Texture3dPtr createRGBAFloat16( int xres = 64, int yres = 64, int zres = 64 );
		static Texture3dPtr createRGBAFloat32( int xres = 64, int yres = 64, int zres = 64 );
	};

	BASE_DECL_SMARTPTR_STRUCT(Texture2dArray);

	struct Texture2dArray
	{
		Texture2dArray();
		~Texture2dArray();

		void   upload( int xres, int yres, int zres, int format, int component, void *data );
		void              uploadRGBA8( int xres, int yres, int zres, unsigned char *pixels );
		void                uploadRGBAFloat32( int xres, int yres, int zres, float *pixels );
		void                    uploadFloat32( int xres, int yres, int zres, float *pixels );
		AttributePtr                                                            getUniform();


		unsigned int                                                                    m_id; // opengl handle
		AttributePtr                                                               m_uniform; // will be used to attach textures to geometry/or shaders which then will be bound
		int                                                           m_xres, m_yres, m_zres; // texture info just for the record
		int                                                                  m_textureFormat; // format of texture in gpu memory

		static Texture2dArrayPtr create( int textureFormat, int xres = 64, int yres = 64, int zres = 64 );
		static Texture2dArrayPtr createRGBA8( int xres = 64, int yres = 64, int zres = 64 );
		static Texture2dArrayPtr createFloat32( int xres = 64, int yres = 64, int zres = 64 );
		static Texture2dArrayPtr createDepth24( int xres = 64, int yres = 64, int zres = 64 );
		static Texture2dArrayPtr createRGBAFloat32( int xres = 64, int yres = 64, int zres = 64 );
		static Texture2dArrayPtr createRGBAFloat16( int xres = 64, int yres = 64, int zres = 64 );
	};

	BASE_DECL_SMARTPTR_STRUCT(TextureCube);

	struct TextureCube
	{
		TextureCube();
		~TextureCube();

		void                                                         upload( ImagePtr image ); // assumes Image contains all 6 faces
		void                                                      upload( ImagePtr faces[6] ); // assumes Image contains all 6 faces
		AttributePtr                                                             getUniform();


		unsigned int                                                                    m_id; // opengl handle
		AttributePtr                                                               m_uniform; // will be used to attach textures to geometry/or shaders which then will be bound
		int                                                                  m_textureFormat; // format of texture in gpu memory

		static TextureCubePtr                      createRGBA8(int xres = 64, int yres = 64);
		static TextureCubePtr                      createSRGBA8(int xres = 64, int yres = 64);
	};

}
