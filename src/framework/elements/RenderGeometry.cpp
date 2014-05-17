#include "RenderGeometry.h"
#include "../houdini/HouGeoIO.h"


RenderGeometry::RenderGeometry() : Element()
{
	addProperty<base::Geometry::Ptr>( "geometry", std::bind( &RenderGeometry::getGeometry, this ), std::bind( &RenderGeometry::setGeometry, this, std::placeholders::_1 ) );
	addProperty<base::Shader::Ptr>( "shader", std::bind( &RenderGeometry::getShader, this ), std::bind( &RenderGeometry::setShader, this, std::placeholders::_1 ) );
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

base::Shader::Ptr RenderGeometry::getShader()const
{
	return m_shader;
}
void RenderGeometry::setShader( base::Shader::Ptr shader )
{
	m_shader = shader;
}






REGISTERCLASS( RenderGeometry )



