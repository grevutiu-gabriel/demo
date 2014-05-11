#pragma once


#include "../Element.h"


class RenderGeometry : public Element
{
	OBJECT
public:
	typedef std::shared_ptr<RenderGeometry> Ptr;
	RenderGeometry() : Element()
	{
	}
	RenderGeometry( base::Geometry::Ptr geometry, base::Shader::Ptr shader ) : Element(), m_geometry(geometry), m_shader(shader)
	{
	}
	static Ptr create()
	{
		return std::make_shared<RenderGeometry>();
	}
	static Ptr create( base::Geometry::Ptr geometry, base::Shader::Ptr shader )
	{
		return std::make_shared<RenderGeometry>(geometry, shader);
	}

	virtual void render(base::Context::Ptr context, float time)override
	{
		context->render(m_geometry, m_shader);
	}


private:
	base::Geometry::Ptr m_geometry;
	base::Shader::Ptr   m_shader;
};

