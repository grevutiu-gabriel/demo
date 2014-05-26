#pragma once

#include <util/shared_ptr.h>
#include <gltools/gl.h>
#include <gltools/misc.h>
#include <util/StringManip.h>
#include <util/Path.h>
#include <gfx/Geometry.h>
#include <gfx/Shader.h>
#include <gfx/Texture.h>
#include <gfx/Image.h>
#include <gfx/Context.h>
#include <gfx/FBO.h>
#include <gfx/Field.h>

#include "TransferFunction.h"

#include "../../Element.h"







class Volume : public Element
{
	OBJECT
public:
	typedef std::shared_ptr<Volume> Ptr;
	struct Light
	{
		Light() : color(1.0f, 1.0f, 1.0f), exposure(1.0f)
		{
		}

		math::Vec3f color;
		float    exposure;
	};

	static Volume::Ptr            create();
	static Volume::Ptr            create( const base::Path dataSourceGLSLPath ); // create volume with custom data source glsl module (file be be loaded from base::fs)

	virtual void render(base::Context::Ptr context, float time)override;
	void                        load( const std::string& filename );



	void                        reload(); // reloads the shaders (for development)

	void setPointLightPosition( math::V3f& pos );
	math::V3f getPointLightPosition()const;
	void setPointLightIntensity( float intensity );
	float getPointLightIntensity()const;
	void setLocalToWorld(const math::M44f& localToWorld);
	math::M44f getLocalToWorld()const;
	void setTransferFunction( AnimatedTransferFunction::Ptr transferFunction );
	AnimatedTransferFunction::Ptr getTransferFunction();

	Volume();

//private:
	base::GeometryPtr           createProxyGeometry();

	void                        updateCrossSectionValues();



	base::Texture2dPtr          volumeBack;
	base::Texture2dPtr          volumeFront;
	base::Texture2dPtr          estimate;
	base::FBOPtr                volumeFrontFBO;
	base::FBOPtr                volumeBackFBO;
	base::FBOPtr                estimateFBO;

	base::FBO::Ptr              blurHFBO;
	base::Texture2dPtr          blurHOut;
	base::FBO::Ptr              blurVFBO;
	base::Texture2dPtr          blurVOut;
	base::Shader::Ptr           blurHShader;
	base::Shader::Ptr           blurVShader;

	base::Shader::Ptr           volumeGeoShader;
	base::Shader::Ptr           volumeShader;

	base::AttributePtr          localToWorldAttr;
	base::AttributePtr          worldToLocalAttr;

	base::GeometryPtr           m_proxy; // proxy geometry
	base::GeometryPtr           nearClipGeo;
	base::AttributePtr          nearClipP;
	base::AttributePtr          nearClipUVW;


	base::Texture3dPtr          m_normalizedDensityTexture;
	base::ScalarField::Ptr      m_normalizedDensity;

	TransferFunction::Ptr       m_transferFunction;
	AnimatedTransferFunction::Ptr       m_transferFunction2;


	base::Texture2dPtr        m_debug;
	base::FBO::Ptr            m_debugFBO;
};
