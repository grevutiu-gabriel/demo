#pragma once


#include "../../Element.h"


struct ChromaticRing
{
	typedef std::shared_ptr<ChromaticRing> Ptr;
	ChromaticRing(base::Attribute::Ptr lightpos);

	static Ptr create( base::Attribute::Ptr lightpos);





	void render( base::Context::Ptr context );
	void updateGeometry();


private:
	float m_thickness;
	float m_brightness;
	float m_spanAngle;
	float m_falloffSpanAngle;
	int m_geoComplexity;
	float m_perpectiveCorrectionFactor;
	base::Geometry::Ptr  m_geometry;
	base::Geometry::Ptr  m_geometryTriangles;
	base::Shader::Ptr    m_shader;
	base::Attribute::Ptr m_lightpos;
	base::Attribute::Ptr m_position;
	base::Attribute::Ptr m_scale;
	base::Texture2d::Ptr m_gradientTexture;
	base::Texture1d::Ptr m_fadeUTexture;
};




class FlareShop : public Element
{
	OBJECT
public:
	typedef std::shared_ptr<FlareShop> Ptr;

	FlareShop();

	static Ptr create();

	virtual void render(base::Context::Ptr context, float time)override;

	void setLightPos( const math::V3f& pos );


private:
	ChromaticRing::Ptr m_ring;
	base::Attribute::Ptr m_lightpos;
};

