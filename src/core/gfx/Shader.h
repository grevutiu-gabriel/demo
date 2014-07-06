#pragma once
#include <gltools/gl.h>
#include <string>
#include <vector>
#include <map>
#include <util/shared_ptr.h>
#include <iostream>

#include "Texture.h"
#include <math/Vec3.h>
#include "Attribute.h"


namespace base
{

	class Shader
	{
	public:
		typedef std::shared_ptr<Shader> Ptr;
		struct ShaderSource;
		typedef std::shared_ptr<ShaderSource> ShaderSourcePtr;

		enum EUniformType
		{
			EUNKNOWN,
			EFLOAT,
			ESAMPLER1D,
			ESAMPLER2D,
			ESAMPLER3D
		};

		struct ShaderLoader
		{
			ShaderLoader( Shader::Ptr shader );

			ShaderLoader                                 attach( int shaderType, ShaderSourcePtr src );
			ShaderLoader                                 attach( int shaderType, const std::string &src);
			ShaderLoader                                 attachFromFile( int shaderType, const std::string &srcFile );
			ShaderLoader                                 attachPS( const std::string &src);
			ShaderLoader                                 attachVS( const std::string &src);
			ShaderLoader                                 attachPSFromFile( const std::string  &src);
			ShaderLoader                                 attachVSFromFile( const std::string  &src);
			ShaderLoader                                 attachPS( const char *src, const int &srcSize );
			ShaderLoader                                 attachVS( const char *src, const int &srcSize );
			ShaderLoader                                 attachPS( ShaderSourcePtr src );
			ShaderLoader                                 attachVS( ShaderSourcePtr src );

			operator                                     Shader::Ptr();
			Shader::Ptr                                  m_shader;
		};

		struct ShaderSource
		{
			static ShaderSourcePtr                       create();
			static ShaderSourcePtr                       create( const std::string &v );
			static ShaderSourcePtr                       createFromFile( const std::string &f );
			void                                         getStringList( std::vector<std::string> &src)const;
			void                                         getFiles( std::vector<std::string>& files ); // fills the list with all involved filenames

			// input methods, order of calls matters
			void                                         file( const std::string &srcFile );
			void                                         verbatim( const std::string &src );
			void                                         define( const std::string &name, const std::string &value = "" );
			void                                         undefine( const std::string &name);

			std::vector<std::pair<int, std::string> >    m_inputs; // source inputs: first int identifies whether the input is a file(1) or verbatim text (0)
			std::map< std::string, std::string >         m_defines; // will go first
		};


		struct ShaderObject
		{
			ShaderObject( int shaderType, ShaderSourcePtr src );
			bool                                         compile();

			int                                          m_shaderType;
			ShaderSourcePtr                              m_src;
			GLhandleARB                                  m_id;
		};
		typedef std::shared_ptr<ShaderObject> ShaderObjectPtr;


		Shader();

		bool                                             isOk();
		void                                             finalize();
		void                                             getAttributeNames( std::vector<std::string> &names );
		void                                             getUniformNames( std::vector<std::string> &names );
		EUniformType                                     getUniformType( const std::string& name );
		void                                             getFiles( std::vector<std::string>& filenames ); // returns a list of all files which belong to that shader


		std::map<std::string, int>                       m_activeAttributes;// list of active attributes (which are required by this shader) (name mapped to index)
		std::map<std::string, int>                       m_activeUniforms;// list of active uniforms (which are required by this shader) (name mapped to index)
		std::map<std::string, Attribute::Ptr>            m_uniforms; // list of uniforms


		// local uniform management ---
		bool                                             hasUniform( const std::string &name );
		Attribute::Ptr                                   getUniform( const std::string &name );
		void                                             setUniform( const std::string &name, Attribute::Ptr uniform );
		void                                             setUniform( const std::string &name, float value ); // convinience function
		void                                             setUniform( const std::string &name, math::Vec2f value ); // convinience function
		void                                             setUniform( const std::string &name, math::Vec3f value ); // convinience function
		void                                             setUniform( const std::string &name, math::Matrix44f value ); // convinience function

		void                                             setUniform( const std::string &name, int value ); // convinience function
		void                                             setUniform( const std::string &name, float v0, float v1, float v2, float v3 ); // convinience function
		void                                             setUniform( const std::string &name, float v0, float v1, float v2 ); // convinience function
		void                                             setUniform( const std::string &name, float v0, float v1 ); // convinience function
		void                                             setUniform( const std::string &name, base::Texture2d::Ptr texture ); // convinience function
		void                                             setUniform( const std::string &name, base::Texture3d::Ptr texture ); // convinience function


		// list of glshaders which are linked into the glprogram
		ShaderObjectPtr                                  createShaderObject( int shaderType, ShaderSourcePtr src );
		std::vector<ShaderObjectPtr>                     m_objects;
		void                                             reload();

		GLhandleARB                                      m_glProgram;
		bool                                             m_isOk;
		std::string                                      m_shaderIdentifier; // some arbitrary string which will be used only for logging so that user knows which shader was compiled

		//
		// creation functions
		//
		static ShaderLoader                              create( const std::string &id = "" );
		static ShaderLoader                              load( const char *vsSrc, const int &vsSrcSize, const char *psSrc, const int &psSrcSize, const std::string &id = "" );
		static ShaderLoader                              loadFromFile( const std::string &shaderBasePath, const std::string &id = "" ); // this method will append .ps.glsl and .vs.glsl  to find vertex and pixelshader
		static ShaderLoader                              loadFromFile( const std::string & vertexShaderPath, const std::string & pixelShaderPath, const std::string &id = "" );
/*
		//
		// some useful default shaders
		//
		static ShaderPtr createSimpleLambertShader();


*/
		static Shader::Ptr                               createSimpleConstantShader( float r = 1.0f, float g = 1.0f, float b = 1.0f );
		static Shader::Ptr                               createSimpleColorShader(); // creates a shader which uses varying Cd attribute
		static Shader::Ptr                               createSimpleTextureShader( Texture2d::Ptr texture = Texture2d::createUVRefTexture() );
		static std::string                               uniformTypeAsString( EUniformType utype );
	};




}
