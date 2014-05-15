#include "RenderGeometry.h"
#include "../houdini/HouGeoIO.h"


RenderGeometry::RenderGeometry() : Element()
{
	std::string basePathData = base::path("data");
	std::string basePathSrc = base::path("src");

	// geometry --------
	{
		base::Geometry::Ptr geo = houdini::HouGeoIO::importGeometry(basePathData + "/test.bgeo");

		base::Shader::Ptr shader = base::Shader::loadFromFile( basePathData + "/matcap" );
		base::Texture2d::Ptr tex = base::Texture2d::load( basePathData + "/scary-light.jpg", GL_SRGB8 );

		base::Context::getCurrentContext()->addTexture2d("droplet_01.png", tex);
		shader->setUniform("tex", tex);

		m_geometry = geo;
		m_shader = shader;
	}
}

void RenderGeometry::render(base::Context::Ptr context, float time)
{
	context->render(m_geometry, m_shader);
}









REGISTERCLASS( RenderGeometry )



