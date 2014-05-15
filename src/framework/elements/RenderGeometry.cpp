#include "RenderGeometry.h"
#include "../houdini/HouGeoIO.h"


RenderGeometry::RenderGeometry() : Element()
{
	std::string basePathData = base::path("data");
	std::string basePathSrc = base::path("src");

	// geometry --------
	{
		base::Shader::Ptr shader = base::Shader::loadFromFile( basePathData + "/matcap" );
		base::Texture2d::Ptr tex = base::Texture2d::load( basePathData + "/scary-light.jpg", GL_SRGB8 );

		base::Context::getCurrentContext()->addTexture2d("droplet_01.png", tex);
		shader->setUniform("tex", tex);

		m_shader = shader;
	}

	addProperty<base::Geometry::Ptr>( "geometry", std::bind( &RenderGeometry::getGeometry, this ), std::bind( &RenderGeometry::setGeometry, this, std::placeholders::_1 ) );
}

void RenderGeometry::render(base::Context::Ptr context, float time)
{
	context->render(m_geometry, m_shader);
}

base::Geometry::Ptr RenderGeometry::getGeometry()const
{
	return m_geometry;
}
void RenderGeometry::setGeometry( base::Geometry::Ptr geometry )
{
	m_geometry = geometry;
}







REGISTERCLASS( RenderGeometry )



