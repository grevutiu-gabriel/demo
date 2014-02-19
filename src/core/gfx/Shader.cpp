#include "Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>

#include "Context.h"
#include <util/fs.h>




namespace base
{

	// ShaderLoader ===============================================================

	Shader::ShaderLoader::ShaderLoader( Shader::Ptr shader ) : m_shader(shader)
	{
	}

	Shader::ShaderLoader Shader::ShaderLoader::attach( int shaderType, ShaderSourcePtr src )
	{
		Shader::ShaderObjectPtr so = m_shader->createShaderObject(shaderType, src);
		so->compile();
		glAttachShader(m_shader->m_glProgram, so->m_id);
		return *this;
	}

	Shader::ShaderLoader Shader::ShaderLoader::attachPS( ShaderSourcePtr src )
	{
		return attach( GL_FRAGMENT_SHADER_ARB, src );
	}

	Shader::ShaderLoader Shader::ShaderLoader::attachVS( ShaderSourcePtr src )
	{
		return attach( GL_VERTEX_SHADER_ARB, src );
	}

	Shader::ShaderLoader Shader::ShaderLoader::attach( int shaderType, const std::string &src)
	{
		return attach( shaderType, ShaderSource::create( src ) );
	}

	Shader::ShaderLoader Shader::ShaderLoader::attachFromFile( int shaderType, const std::string &srcFile )
	{
		return attach( shaderType, ShaderSource::createFromFile(srcFile) );
	}

	Shader::ShaderLoader Shader::ShaderLoader::attachPSFromFile( const std::string & srcFile )
	{
		return attachFromFile( GL_FRAGMENT_SHADER_ARB, srcFile );
	}

	Shader::ShaderLoader Shader::ShaderLoader::attachVSFromFile( const std::string & srcFile )
	{
		return attachFromFile( GL_VERTEX_SHADER_ARB, srcFile );
	}

	Shader::ShaderLoader Shader::ShaderLoader::attachPS( const std::string &src )
	{
		return attach( GL_FRAGMENT_SHADER_ARB, src );
	}

	Shader::ShaderLoader Shader::ShaderLoader::attachVS( const std::string &src )
	{
		return attach( GL_VERTEX_SHADER_ARB, src );
	}

	Shader::ShaderLoader Shader::ShaderLoader::attachPS( const char *src, const int &srcSize )
	{
		std::string ps_src( src, srcSize );
		return attach( GL_FRAGMENT_SHADER_ARB, ps_src );
	}

	Shader::ShaderLoader Shader::ShaderLoader::attachVS( const char *src, const int &srcSize )
	{
		std::string vs_src( src, srcSize );
		return attach( GL_VERTEX_SHADER_ARB, vs_src );
	}

	Shader::ShaderLoader::operator Shader::Ptr()
	{
		// if shader has no id yet, create one from shader sources
		if( m_shader->m_shaderIdentifier == "" )
		{
			std::string id = "";
			//m_shader->m_objects
			for( std::vector<Shader::ShaderObjectPtr>::iterator it = m_shader->m_objects.begin(), end = m_shader->m_objects.end(); it != end; ++it )
			{
				Shader::ShaderObjectPtr so = *it;
				for( std::vector< std::pair<int, std::string> >::const_iterator it = so->m_src->m_inputs.begin(); it != so->m_src->m_inputs.end(); ++it )
				{
					int type = it->first;
					const std::string &str = it->second;

					if( type == 1 )
					{
						// file
						id = str;
						break;
					}
				}
				if( id != "" )
					break;
			}
			m_shader->m_shaderIdentifier = id;
		}
		m_shader->finalize();

		if( m_shader->isOk() )
			return m_shader;

		return Shader::Ptr();
	}


	Shader::ShaderLoader Shader::load( const char *vsSrc, const int &vsSrcSize, const char *psSrc, const int &psSrcSize, const std::string &id )
	{
		return Shader::create( id ).attachVS( vsSrc, vsSrcSize ).attachPS( psSrc, psSrcSize );
	}


	// ShaderSource ===========================================================================

	Shader::ShaderSourcePtr Shader::ShaderSource::create()
	{
		return std::make_shared<Shader::ShaderSource>();
	}

	Shader::ShaderSourcePtr Shader::ShaderSource::create( const std::string &v )
	{
		ShaderSourcePtr ss = std::make_shared<Shader::ShaderSource>();
		ss->verbatim( v );
		return ss;
	}

	Shader::ShaderSourcePtr Shader::ShaderSource::createFromFile( const std::string &f )
	{
		ShaderSourcePtr ss = std::make_shared<Shader::ShaderSource>();
		ss->file( f );
		return ss;

	}

	void Shader::ShaderSource::file( const std::string &srcFile )
	{
		m_inputs.push_back( std::make_pair(1, srcFile) );
	}

	void Shader::ShaderSource::verbatim( const std::string &src )
	{
		m_inputs.push_back( std::make_pair(0, src) );
	}

	void Shader::ShaderSource::define( const std::string &name, const std::string &value )
	{
		m_defines[name] = value;
	}

	void Shader::ShaderSource::undefine( const std::string &name)
	{
		std::map<std::string, std::string>::iterator it = m_defines.find(name);
		if( it != m_defines.end() )
			m_defines.erase(it);
	}


	void Shader::ShaderSource::getStringList( std::vector<std::string> &src) const
	{
		// put all defines in front
		for( std::map<std::string, std::string>::const_iterator it = m_defines.begin(), end = m_defines.end(); it != end; ++it  )
			src.push_back( "#define " + it->first + " " + it->second + "\n");

		for( std::vector< std::pair<int, std::string> >::const_iterator it = m_inputs.begin(); it != m_inputs.end(); ++it )
		{
			int type = it->first;
			const std::string &str = it->second;

			if( type==0 )
				//verbatim
				src.push_back( str );
			else if( type == 1 )
				// file
				src.push_back( base::fs::read( str ) );
		}
	}


	// ShaderObject ===========================================================================


	Shader::ShaderObject::ShaderObject(  int shaderType, ShaderSourcePtr src  ) : m_shaderType(shaderType), m_src(src)
	{
		m_id = glCreateShader(m_shaderType);
	}
	
	bool Shader::ShaderObject::compile()
	{
		std::vector<std::string> src;
		m_src->getStringList(src); // this will reload content from file inputs

		// convert to c-style pointers
		std::vector<const char *> src_c;
		std::vector<int> length;

		for( std::vector<std::string>::iterator it = src.begin(); it != src.end();++it )
		{
			const std::string &cur = *it;
			length.push_back( (int)cur.size() );
			src_c.push_back( cur.c_str() );
		}

		glShaderSource(m_id, (int)src.size(), (const GLchar **)&src_c[0], &length[0]);
		glCompileShader(m_id);

		//TODO: error check

		return true;
	}



	// Shader ==============================================================

	void Shader::reload()
	{
		// iterate all shader objects, recompile them
		for( std::vector<ShaderObjectPtr>::iterator it = m_objects.begin(); it != m_objects.end(); ++it )
			(*it)->compile();

		// then relink
		finalize();
	}

	Shader::ShaderLoader Shader::loadFromFile( const std::string &vertexShaderPath, const std::string &pixelShaderPath, const std::string &id )
	{
		return Shader::create( id ).attachFromFile( GL_VERTEX_SHADER_ARB, vertexShaderPath ).attachFromFile( GL_FRAGMENT_SHADER_ARB, pixelShaderPath );
	}

	// this method will append .ps.glsl and .vs.glsl  to find vertex and pixelshader
	Shader::ShaderLoader Shader::loadFromFile( const std::string &shaderBasePath, const std::string &id )
	{
		Shader::ShaderLoader loader = Shader::create( id );

		loader.attachFromFile( GL_VERTEX_SHADER_ARB, shaderBasePath + ".vs.glsl" );
		loader.attachFromFile( GL_FRAGMENT_SHADER_ARB, shaderBasePath + ".ps.glsl" );

		std::string geometryShaderPath = shaderBasePath + ".gs.glsl";
		if( fs::exists(geometryShaderPath) )
			loader.attachFromFile(GL_GEOMETRY_SHADER_EXT, geometryShaderPath);

		return loader;
	}


	Shader::ShaderLoader Shader::create( const std::string &id )
	{
		Shader::Ptr shader = std::make_shared<Shader>();
		shader->m_shaderIdentifier = id;
		return Shader::ShaderLoader( shader );
	}

	Shader::ShaderObjectPtr Shader::createShaderObject( int shaderType, ShaderSourcePtr src )
	{
		ShaderObjectPtr so = std::make_shared<ShaderObject>( shaderType, src );
		m_objects.push_back( so );
		return so;
	}

	Shader::Shader() : m_isOk(false), m_shaderIdentifier("")
	{
		m_glProgram = glCreateProgram();
	}

	bool Shader::isOk()
	{
		return m_isOk;
	}


	void Shader::getAttributeNames( std::vector<std::string> &names )
	{
		names.clear();
		for( std::map<std::string, int>::iterator it = m_activeAttributes.begin(), end = m_activeAttributes.end(); it != end; ++it )
			names.push_back(it->first);
	}

	void Shader::finalize()
	{
		char text[1000];
		//
		// bind attribute locations
		//
		glBindAttribLocation(m_glProgram, 0, "P");
		glBindAttribLocation(m_glProgram, 1, "N");
		glBindAttribLocation(m_glProgram, 2, "Cd");
		glBindAttribLocation(m_glProgram, 3, "W");
		glBindAttribLocation(m_glProgram, 4, "CMT");
		glBindAttribLocation(m_glProgram, 5, "BW");
		glBindAttribLocation(m_glProgram, 6, "BI");
		glBindAttribLocation(m_glProgram, 7, "UV");

		//
		// program linking --------
		//
		glLinkProgram(m_glProgram);
		glGetInfoLogARB(m_glProgram, 1000, 0, text);
		std::cout << "Shader::finalize: "<< m_shaderIdentifier << text << std::endl;


		// extract active attributes info
		int numActiveAttributes = 0;
		m_activeAttributes.clear();
		glGetProgramiv(m_glProgram, GL_ACTIVE_ATTRIBUTES, &numActiveAttributes);
		std::cout << "\tnumber of active attributes: " << numActiveAttributes << std::endl;
		for( int i=0;i<numActiveAttributes; ++i )
		{
			char name[1000];
			int length;
			int size;
			unsigned int type;
			glGetActiveAttrib( m_glProgram, i, 1000, &length, &size, &type, name );
			int index = glGetAttribLocation(m_glProgram, name);
			std::cout << "\tactive attribute " << name << " at location " << index << std::endl;
			m_activeAttributes.insert(std::make_pair( std::string(name), index ));
		}

		// extract active uniforms info
		int numActiveUniforms = 0;
		m_activeUniforms.clear();
		glGetProgramiv(m_glProgram, GL_ACTIVE_UNIFORMS, &numActiveUniforms);
		std::cout << "\tnumber of active uniforms: " << numActiveUniforms << std::endl;
		for( int i=0;i<numActiveUniforms; ++i )
		{
			char name[1000];
			int length;
			int size;
			unsigned int type;
			glGetActiveUniform( m_glProgram, i, 1000, &length, &size, &type, name );
			int index = glGetUniformLocation(m_glProgram, name);
			std::cout << "\tactive uniform " << name << " at location " << index << std::endl;

			// index==-1 means uniform is a built in uniform and we dont touch it
			if( index != -1 )
			{

				// fucking ati laptop cards puts an [i] at the end of array uniforms. have
				// to remove that to remain compatible with the other shit
				int l = (int)strlen( name );
				if( (name[l-3] == '[') && (name[l-2] == '0') && (name[l-1] == ']') )
					name[l-3] = '\0';

				m_activeUniforms.insert( std::make_pair( std::string(name), index) );
			}
		}

		m_isOk = true;
	}


	bool Shader::hasUniform( const std::string &name )
	{
		return (m_uniforms.find( name ) != m_uniforms.end());
	}

	Attribute::Ptr Shader::getUniform( const std::string &name )
	{
		std::map<std::string, Attribute::Ptr>::iterator it = m_uniforms.find( name );
		if(it != m_uniforms.end())
			return it->second;
		return Attribute::Ptr();
	}

	void Shader::setUniform( const std::string &name, Attribute::Ptr uniform )
	{
		m_uniforms[name] = uniform;
	}

	void Shader::setUniform( const std::string &name, float value )
	{
		Attribute::Ptr u = getUniform(name);
		if(!u)
		{
			u = Attribute::createFloat(1);
			setUniform(name, u);
		}
		u->set<float>(0, value );
	}


	void Shader::setUniform( const std::string &name, float v0, float v1 )
	{
		AttributePtr u = getUniform(name);
		if(!u)
		{
			u = Attribute::createV2f();
			u->appendElement<float>(v0,v1);
			setUniform(name, u);
		}else
			u->set<float>(0, v0, v1);
	}

	void Shader::setUniform( const std::string &name, float v0, float v1, float v2 )
	{
		AttributePtr u = getUniform(name);
		if(!u)
		{
			u = Attribute::createV3f();
			u->appendElement<float>(v0,v1,v2);
			setUniform(name, u);
		}else
			u->set<float>(0, v0, v1, v2);
	}

	void Shader::setUniform( const std::string &name, float v0, float v1, float v2, float v3 )
	{
		AttributePtr u = getUniform(name);
		if(!u)
		{
			u = Attribute::createV4f();
			u->appendElement<float>(v0,v1,v2,v3);
			setUniform(name, u);
		}else
			u->set<float>(0, v0, v1, v2, v3);
	}

	void Shader::setUniform( const std::string &name, math::Vec2f value )
	{
		AttributePtr u = getUniform(name);
		if(!u)
		{
			u = Attribute::createV2f();
			u->appendElement<math::Vec2f>(value);
			setUniform(name, u);
		}else
			u->set<math::Vec2f>(0, value);
	}

	void Shader::setUniform( const std::string &name, math::Vec3f value )
	{
		AttributePtr u = getUniform(name);
		if(!u)
		{
			u = Attribute::createV3f();
			u->appendElement<math::Vec3f>(value);
			setUniform(name, u);
		}else
			u->set<math::Vec3f>(0, value);
	}

	void Shader::setUniform( const std::string &name, math::Matrix44f value )
	{
		AttributePtr u = getUniform(name);
		if(!u)
		{
			u = Attribute::createM44f();
			u->appendElement<math::Matrix44f>(value);
			setUniform(name, u);
		}else
			u->set<math::Matrix44f>(0, value);
	}


	void Shader::setUniform( const std::string &name, int value )
	{
		AttributePtr u = getUniform(name);
		if(!u)
		{
			u = Attribute::createInt();
			u->appendElement<int>(value);
			setUniform(name, u);
		}else
			u->set<int>(0, value);
	}


/*
	ShaderPtr Shader::createSimpleLambertShader()
	{
		ShaderPtr shader = Shader::loadFromFile("$APPDATA/glsl/simpleLambert");
		shader->setUniform( "l", math::Vec3f( 1.0f, 1.0f, 1.0f ).normalized() );
		shader->setUniform( "diffuse", math::Vec3f(0.5f, 0.5f, 0.5f));
		shader->setUniform( "kd", 1.0f );
		shader->setUniform( "ambient", math::Vec3f(0.05f, 0.05f, 0.05f));
		shader->setUniform( "ka", 1.0f );
		shader->setUniform( "l", math::Vec3f(1.0f, 1.0f, 1.0f).normalized() );
		return shader;
	}

	ShaderPtr Shader::createSimpleTextureShader( Texture2dPtr texture )
	{
		ShaderPtr shader = Shader::create().attachPSFromFile("$APPDATA/glsl/simpleTexture.ps.glsl").attachVSFromFile( "$APPDATA/data/glsl/simpleTexture.vs.glsl");
		if(texture)
			shader->setUniform( "texture", texture->getUniform() );
		return shader;
	}

	ShaderPtr Shader::createSimpleConstantShader( float r, float g, float b )
	{
		ShaderPtr shader = Shader::create().attachPSFromFile( "$APPDATA/glsl/constant.ps.glsl").attachVSFromFile( "$APPDATA/glsl/constant.vs.glsl");
		shader->setUniform( "color", math::Vec3f(r, g, b) );
		return shader;
	}

*/
	// creates a shader which uses varying Cd attribute
	Shader::Ptr Shader::createSimpleColorShader()
	{
		unsigned char vertexShaderSource[] =
			"#version 400 core\n"
			"in vec3 P;\n"
			"in vec3 Cd;\n"
			"out vec3 cd;\n"
			"uniform mat4 mvpm;\n"
			"void main(void)\n"
			"{\n"
			"\tcd = Cd;\n"
			"\tgl_Position = mvpm * vec4(P,1.0);\n"
			"}\n";
		std::stringstream vs_src;
		vs_src << vertexShaderSource;


		unsigned char pixelShaderSource[] =
			"#version 400 core\n"
			"in vec3 cd;\n"
			"out vec4 color;\n"
			"void main()\n"
			"{\n"
			"\tcolor = vec4( cd, 1.0 );\n"
			"\t//color = vec4( 0.3, 0.1, 0.5, 1.0 );\n"
			"}\n";

		std::stringstream ps_src;
		ps_src << pixelShaderSource;


		ShaderLoader sl = Shader::create("simple color");
		sl.attachVS( vs_src.str() );
		sl.attachPS( ps_src.str() );
		return sl;
	}

	Shader::Ptr Shader::createSimpleTextureShader( Texture2d::Ptr texture )
	{
		static std::string g_vs =
				"#version 330\n"
				"in vec3 P;"
				"in vec2 UV;"
				"out vec2 uv;"
				"uniform mat4 mvpm;"
				"void main()"
				"{"
				"    uv = UV;"
				"    gl_Position = mvpm * vec4(P,1.0);"
				"}";
		static std::string g_ps =
				"#version 330\n"
				"in vec2 uv;"
				"uniform sampler2D texture;"
				"layout(location = 0) out vec4 frag_color;"
				"void main()"
				"{"
				"    frag_color = texture2D(texture,uv);"
				"}";
		Shader::Ptr shader;
		shader = Shader::create("Shader::simpleTextureShader").attachPS(g_ps).attachVS(g_vs);
		if(texture)
			shader->setUniform( "texture", texture->getUniform() );
		return shader;
	}
}



