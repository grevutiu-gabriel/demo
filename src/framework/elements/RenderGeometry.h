#pragma once


#include "../Element.h"


class RenderGeometry : public Element
{
	OBJECT
public:
	typedef std::shared_ptr<RenderGeometry> Ptr;
	RenderGeometry();
	static Ptr create()
	{
		return std::make_shared<RenderGeometry>();
	}

	virtual void render(base::Context::Ptr context, float time)override;


private:
	base::Geometry::Ptr m_geometry;
	base::Shader::Ptr   m_shader;
};

