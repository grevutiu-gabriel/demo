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

	base::Geometry::Ptr getGeometry()const;
	void setGeometry( base::Geometry::Ptr geometry );
	base::Shader::Ptr getShader()const;
	void setShader( base::Shader::Ptr shader );
	math::M44f getTransform() const;
	void setTransform(const math::M44f &transform);

private:
	base::Geometry::Ptr m_geometry;
	base::Shader::Ptr   m_shader;
	math::M44f          m_transform;
};

