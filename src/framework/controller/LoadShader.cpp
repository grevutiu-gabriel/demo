#include "LoadShader.h"

#include <util/Path.h>
#include <gfx/Context.h>
#include <util/fs.h>

#include "../houdini/HouGeoIO.h"


LoadShader::LoadShader() : ShaderController()
{

}

base::Shader::Ptr LoadShader::evaluate(float time)
{
	return m_shader;
}

bool LoadShader::isAnimated()const
{
	return false;
}

void LoadShader::setFilename( const std::string& filename )
{
	// load the file immediately
	m_filename = filename;

	std::string expanded = base::expand(m_filename);

	if(!(base::fs::exists(expanded + ".vs.glsl") && base::fs::exists(expanded + ".ps.glsl")))
		return;

	// shader --------
	{
		m_shader = base::Shader::loadFromFile( base::expand(m_filename) );
		updateProperties();
	}
}


std::string LoadShader::getFilename()
{
	return m_filename;
}

void LoadShader::setUniformSampler2D(const std::string &name, base::Texture2d::Ptr texture)
{
	m_shader->setUniform(name, texture);
}

void LoadShader::serialize(Serializer &out)
{
	ShaderController::serialize(out);

	out.write( "filename", m_filename );

	// serialize uniforms
	if(m_shader->isOk())
	{
		// perform introspection on all uniforms
		std::vector<std::string> uniformNames;
		m_shader->getUniformNames(uniformNames);

		for( auto name:uniformNames )
		{
			switch( m_shader->getUniformType(name) )
			{
				case base::Shader::EUNKNOWN:
				{
				}break;
				case base::Shader::EFLOAT:
				{
					out.write( name, m_shader->getUniform(name)->get<float>(0) );
				}break;
				case base::Shader::ESAMPLER1D:
				{
				}break;
				case base::Shader::ESAMPLER2D:
				{
				}break;
				case base::Shader::ESAMPLER3D:
				{
				}break;
			};
		}
	}

}

void LoadShader::deserialize(Deserializer &in)
{
	ShaderController::deserialize(in);
	setFilename( in.readString("filename") );

	// deserialize uniforms
	if(m_shader->isOk())
	{
		// perform introspection on all uniforms
		std::vector<std::string> uniformNames;
		m_shader->getUniformNames(uniformNames);

		for( auto name:uniformNames )
		{
			switch( m_shader->getUniformType(name) )
			{
				case base::Shader::EUNKNOWN:
				{
				}break;
				case base::Shader::EFLOAT:
				{
					m_shader->getUniform(name)->set<float>(0, in.readFloat(name));
				}break;
				case base::Shader::ESAMPLER1D:
				{
				}break;
				case base::Shader::ESAMPLER2D:
				{
				}break;
				case base::Shader::ESAMPLER3D:
				{
				}break;
			};
		}
	}
}

void LoadShader::reload()
{
	m_shader->reload();
	updateProperties();
}

void LoadShader::updateProperties()
{
	clearProperties();

	if(!m_shader->isOk())
		return;

	// perform introspection on all uniforms
	std::vector<std::string> uniformNames;
	m_shader->getUniformNames(uniformNames);

	for( auto name:uniformNames )
	{
		std::cout << "got uniform: " << name << " (" << base::Shader::uniformTypeAsString(m_shader->getUniformType(name)) << ")" << std::endl;
		switch( m_shader->getUniformType(name) )
		{
			case base::Shader::EUNKNOWN:
			{
			}break;
			case base::Shader::EFLOAT:
			{
				// create a float property
				m_shader->setUniform(name, 0.0f);
				addProperty<float>( name, [=]{return m_shader->getUniform(name)->get<float>(0);}, std::bind( static_cast<void(base::Shader::*)(const std::string& name, float)>(&base::Shader::setUniform), m_shader, name, std::placeholders::_1 ) );
			}break;
			case base::Shader::ESAMPLER1D:
			{
			}break;
			case base::Shader::ESAMPLER2D:
			{
				// create a texture2d property
				addProperty<base::Texture2d::Ptr>( name, PropertyT<base::Texture2d::Ptr>::Getter(), std::bind( static_cast<void(base::Shader::*)(const std::string& name, base::Texture2d::Ptr)>(&base::Shader::setUniform), m_shader, name, std::placeholders::_1 ) );
			}break;
			case base::Shader::ESAMPLER3D:
			{

			}break;
		};

		m_props.push_back(name);
	}
}

void LoadShader::clearProperties()
{
	for( auto prop : m_props )
		removeProperty( prop );
	m_props.clear();
}

REGISTERCLASS2( LoadShader, Controller )





