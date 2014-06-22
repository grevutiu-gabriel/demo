#include "RenderGeometry.h"
#include "../houdini/HouGeoIO.h"


RenderGeometry::RenderGeometry() :
	Element(),
	m_transform(math::M44f::Identity())
{
	addProperty<base::Geometry::Ptr>( "geometry", std::bind( &RenderGeometry::getGeometry, this ), std::bind( &RenderGeometry::setGeometry, this, std::placeholders::_1 ) );
	addProperty<base::Shader::Ptr>( "shader", std::bind( &RenderGeometry::getShader, this ), std::bind( &RenderGeometry::setShader, this, std::placeholders::_1 ) );
	addProperty<math::M44f>( "transform", std::bind( &RenderGeometry::getTransform, this ), std::bind( &RenderGeometry::setTransform, this, std::placeholders::_1 ) );
}

void RenderGeometry::render(base::Context::Ptr context, float time)
{
	if( m_geometry && m_shader )
		context->render(m_geometry, m_shader, m_transform);
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

math::M44f RenderGeometry::getTransform() const
{
	return m_transform;
}

void RenderGeometry::setTransform(const math::M44f &transform)
{
	m_transform = transform;
}







REGISTERCLASS2( RenderGeometry, Element )



