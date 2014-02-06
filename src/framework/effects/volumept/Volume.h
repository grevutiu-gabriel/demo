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






BASE_DECL_SMARTPTR_STRUCT(Volume);
struct Volume
{
	typedef std::shared_ptr<Volume> Ptr;
	struct Light
	{
		Light() : color(1.0f, 1.0f, 1.0f), exposure(1.0f)
		{
		}

		math::Vec3f color;
		float    exposure;
	};

	static VolumePtr            create();
	static VolumePtr            create( const base::Path dataSourceGLSLPath ); // create volume with custom data source glsl module (file be be loaded from base::fs)

	void                        render(base::Context::Ptr context, base::Camera::Ptr cam );
	void                        load( const std::string& filename );



	void                        reload(); // reloads the shaders (for development)
	void                        setUniform( const std::string &name, base::AttributePtr uniform ); // to allow external code to communicate with glsldatasource module

	void                        setTotalCrossSection( float totalCrossSection );
	float                       getTotalCrossSection( void );
	void                        setAlbedo( float albedo );
	float                       getAlbedo( void );
	void                        setAbsorptionColor( math::Vec3f absorptionColor );
	math::Vec3f                 getAbsorptionColor( void );
	void                        setScatteringColor( math::Vec3f scatteringColor );
	math::Vec3f                 getScatteringColor( void );


	Volume();

//private:
	void                        initialize();
	base::GeometryPtr           createProxyGeometry();

	void                        updateCrossSectionValues();


	// local (cached) variables
	float                  m_totalCrossSection;
	float                             m_albedo;
	math::Vec3f              m_absorptionColor;
	math::Vec3f              m_scatteringColor;

	base::Texture2dPtr          volumeBack;
	base::Texture2dPtr          volumeFront;
	base::FBOPtr                volumeFrontFBO;
	base::FBOPtr                volumeBackFBO;

	base::ShaderPtr             volumeGeoShader;
	base::ShaderPtr             volumeShader;

	base::AttributePtr          localToWorldAttr;
	base::AttributePtr          worldToLocalAttr;

	base::GeometryPtr           m_proxy; // proxy geometry
	base::GeometryPtr           nearClipGeo;
	base::AttributePtr          nearClipP;
	base::AttributePtr          nearClipUVW;


	base::Texture3dPtr          m_densityTexture;
	base::ScalarField::Ptr      m_density;

	TransferFunction::Ptr       m_transferFunction;

	base::Texture2dPtr        m_debug;
	base::FBO::Ptr            m_debugFBO;
};
