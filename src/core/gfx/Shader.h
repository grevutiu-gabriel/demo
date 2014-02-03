#pragma once


#include <string>
#include <vector>
#include <map>
#include <iostream>

#include <gltools/gl.h>
#include <util/shared_ptr.h>
#include <util/Path.h>
#include <math/Vec3.h>
#include "Attribute.h"
#include "Texture.h"


namespace base
{
	BASE_DECL_SMARTPTR_STRUCT(Shader)

	struct Shader
	{
		typedef std::shared_ptr<Shader> Ptr;
		BASE_DECL_SMARTPTR_STRUCT(ShaderSource)

		struct ShaderLoader
		{
			ShaderLoader( ShaderPtr shader );

			ShaderLoader attach( int shaderType, ShaderSourcePtr src );
			ShaderLoader attach( int shaderType, const std::string &src);
			ShaderLoader attach( int shaderType, Path srcFile );
			ShaderLoader attachPS( const std::string &src);
			ShaderLoader attachVS( const std::string &src);
			ShaderLoader attachPS( Path src);
			ShaderLoader attachVS( Path src);
			ShaderLoader attachPS( const char *src, const int &srcSize );
			ShaderLoader attachVS( const char *src, const int &srcSize );
			ShaderLoader attachPS( ShaderSourcePtr src );
			ShaderLoader attachVS( ShaderSourcePtr src );

			operator ShaderPtr ();
			ShaderPtr    m_shader;
		};

		struct ShaderSource
		{
			static ShaderSourcePtr                                        create();
			static ShaderSourcePtr                  create( const std::string &v );
			static ShaderSourcePtr                               create( Path &f );
			void                getStringList( std::vector<std::string> &src)const;

			// input methods, order of calls matters
			void                                              file( Path srcFile );
			void                                verbatim( const std::string &src );
			void  define( const std::string &name, const std::string &value = "" );
			void                                undefine( const std::string &name);

			std::vector<std::pair<int, std::string> >                     m_inputs; // source inputs: first int identifies whether the input is a file(1) or verbatim text (0)
			std::map< std::string, std::string >                         m_defines; // will go first
		};

		BASE_DECL_SMARTPTR_STRUCT(ShaderObject)
		struct ShaderObject
		{
			ShaderObject( int shaderType, ShaderSourcePtr src );
			bool                                      compile();

			int                                    m_shaderType;
			ShaderSourcePtr                               m_src;
			GLhandleARB                                    m_id;
		};


		Shader();

		//void init(const std::string &vsSrc, const std::string &psSrc, const std::string &id = "");
		bool isOk();
		void finalize();



		// set uniforms
		// set vertex attributes
		// list of active attributes (which are required by this shader) (name mapped to index)
		std::map<std::string, int> m_activeAttributes;
		// list of active uniforms (which are required by this shader) (name mapped to index)
		std::map<std::string, int> m_activeUniforms;


		//
		// local uniform management
		//
		void                      setUniform( const std::string &name, AttributePtr uniform );
		void                         setUniform( const std::string &name, math::Vec2f value ); // convinience function
		void                         setUniform( const std::string &name, math::Vec3f value ); // convinience function
		void                     setUniform( const std::string &name, math::Matrix44f value ); // convinience function
		void                               setUniform( const std::string &name, float value ); // convinience function
		void                                 setUniform( const std::string &name, int value ); // convinience function
		void    setUniform( const std::string &name, float v0, float v1, float v2, float v3 ); // convinience function
		void              setUniform( const std::string &name, float v0, float v1, float v2 ); // convinience function
		void                        setUniform( const std::string &name, float v0, float v1 ); // convinience function
		bool                                            hasUniform( const std::string &name );
		AttributePtr                                    getUniform( const std::string &name );
		std::map<std::string, AttributePtr>                                        m_uniforms; // list of uniforms



		// list of glshaders which are linked into the glprogram
		ShaderObjectPtr             createShaderObject( int shaderType, ShaderSourcePtr src );
		std::vector<ShaderObjectPtr>                                                m_objects;
		void                                                                         reload();

		GLhandleARB               m_glProgram;
		bool                           m_isOk;
		std::string        m_shaderIdentifier; // some arbitrary string which will be used only for logging so that user knows which shader was compiled

		//
		// creation functions
		//
		static ShaderLoader create( const std::string &id = "" );
		static ShaderLoader load( const char *vsSrc, const int &vsSrcSize, const char *psSrc, const int &psSrcSize, const std::string &id = "" );
		static ShaderLoader load( Path shaderBasePath, const std::string &id = "" ); // this method will append .ps.glsl and .vs.glsl  to find vertex and pixelshader
		static ShaderLoader load( Path vertexShaderPath, Path pixelShaderPath, const std::string &id = "" );

		//
		// some useful default shaders
		//
		static Ptr createSimpleTextureShader( Texture2d::Ptr texture = Texture2d::Ptr() );
//		static ShaderPtr createSimpleLambertShader();
//		static ShaderPtr createSimpleConstantShader( float r = 1.0f, float g = 1.0f, float b = 1.0f );
//		static ShaderPtr createSimpleColorShader(); // creates a shader which uses varying Cd attribute


	};

}
