#pragma once


#include "../Element.h"


struct RenderGeometry : public Element
{
	typedef std::shared_ptr<RenderGeometry> Ptr;

	RenderGeometry( base::Geometry::Ptr geometry, base::Shader::Ptr shader ) : Element(), m_geometry(geometry), m_shader(shader)
	{
	}

	static Ptr create( base::Geometry::Ptr geometry, base::Shader::Ptr shader )
	{
		return std::make_shared<RenderGeometry>(geometry, shader);
	}

	virtual void render(base::Context::Ptr context)override
	{
		context->render(m_geometry, m_shader);
	}


private:
	base::Geometry::Ptr m_geometry;
	base::Shader::Ptr   m_shader;
};

