#include "LoadShader.h"

#include <util/Path.h>
#include <gfx/Context.h>

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

	std::string basePathData = base::path("data");
	std::string basePathSrc = base::path("src");

	// geometry --------
	{
		m_shader = base::Shader::loadFromFile( base::expand(m_filename) );

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
		}

//		base::Texture2d::Ptr tex = base::Texture2d::load( basePathData + "/scary-light.jpg", GL_SRGB8 );
//		base::Context::getCurrentContext()->addTexture2d("droplet_01.png", tex);
//		m_shader->setUniform("tex", tex);
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
}

REGISTERCLASS( LoadShader )





