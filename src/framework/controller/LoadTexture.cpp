#include "LoadTexture.h"

#include <util/Path.h>
#include <gfx/Context.h>



LoadTexture::LoadTexture() : TextureController()
{

}

base::Texture2d::Ptr LoadTexture::evaluate(float time)
{
	return m_texture;
}

bool LoadTexture::isAnimated()const
{
	return false;
}

void LoadTexture::setFilename( const std::string& filename )
{
	// load the file immediately
	m_filename = filename;



	m_texture = base::Texture2d::load( base::expand(m_filename), GL_SRGB8 );
}


std::string LoadTexture::getFilename()
{
	return m_filename;
}


void LoadTexture::serialize(Serializer &out)
{
	TextureController::serialize(out);

	out.write( "filename", m_filename );
}

REGISTERCLASS( LoadTexture )





