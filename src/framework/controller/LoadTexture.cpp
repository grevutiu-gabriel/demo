#include "LoadTexture.h"

#include <util/Path.h>
#include <gfx/Context.h>



LoadTexture2d::LoadTexture2d() : Texture2dController()
{

}

base::Texture2d::Ptr LoadTexture2d::evaluate(float time)
{
	return m_texture;
}

bool LoadTexture2d::isAnimated()const
{
	return false;
}

void LoadTexture2d::setFilename( const std::string& filename )
{
	// load the file immediately
	m_filename = filename;



	m_texture = base::Texture2d::load( base::expand(m_filename), GL_SRGB8 );
}


std::string LoadTexture2d::getFilename()
{
	return m_filename;
}




void LoadTexture2d::serialize(Serializer &out)
{
	Texture2dController::serialize(out);

	out.write( "filename", m_filename );
}

void LoadTexture2d::deserialize(Deserializer &in)
{
	Texture2dController::deserialize(in);
	setFilename(in.readString("filename"));
}



// -----------------------------

LoadTexture3d::LoadTexture3d() : Texture3dController()
{

}

base::Texture3d::Ptr LoadTexture3d::evaluate(float time)
{
	return m_texture;
}

bool LoadTexture3d::isAnimated()const
{
	return false;
}

void LoadTexture3d::setFilename( const std::string& filename )
{
	// load the file immediately
	m_filename = filename;



	//m_texture = base::Texture3d::load( base::expand(m_filename), GL_SRGB8 );
}


std::string LoadTexture3d::getFilename()
{
	return m_filename;
}


void LoadTexture3d::serialize(Serializer &out)
{
	Texture3dController::serialize(out);

	out.write( "filename", m_filename );
}


REGISTERCLASS( LoadTexture2d )
REGISTERCLASS( LoadTexture3d )





