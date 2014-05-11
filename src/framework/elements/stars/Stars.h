#pragma once

#include <util/shared_ptr.h>
#include <util/StringManip.h>
#include <util/Path.h>
#include <util/fs.h>

#include <gfx/Camera.h>
#include <gfx/Geometry.h>
#include <gfx/Shader.h>
#include <gfx/Texture.h>
#include <gfx/Image.h>
#include <gfx/Context.h>


#include "../../Element.h"

#include <math/Half/half.h>

class Stars : public Element
{
	OBJECT
public:
	typedef std::shared_ptr<Stars> Ptr;


	Stars();

	static Ptr                        create();

	virtual void                      render( base::Context::Ptr context, float time )override;

	void                              setStarBrightnessScale( float starBrightnessScale );
	float                             getStarBrightnessScale()const;
	void                              setDomeBrightnessScale( float domeBrightnessScale );
	float                             getDomeBrightnessScale()const;

	void                              setMotionblurScale( float mbscale );
	float                             getMotionblurScale()const;

	void                              setFixedRotationEnabled( bool enabled );
	bool                              isFixedRotationEnabled()const;
	void                              setFixedRotation( const math::Vec3f &rotationAxis, const float &rotationAngle );




	base::GeometryPtr                 m_stars;
	base::GeometryPtr                 m_dome;
	base::Shader::Ptr                 m_domeShader;
	base::Shader::Ptr                 m_starsShader;

	base::TextureCubePtr                                                     domeCubeMap;

	math::Matrix44f                                                               m_mvm0; // previous modelview transform (orientation only)
	base::Texture1dPtr                                                           m_aaTex; // anti aliasing lookup table
	float                                                                   m_currentFov;
	int                                                            m_currentScreenHeight;

	float                                                          m_starBrightnessScale;
	float                                                          m_domeBrightnessScale;

	float                                                           m_motionblurStrength;

	bool                                                                 m_fixedRotation;
	float                                                           m_fixedRotationAngle;
	math::Vec3f                                                      m_fixedRotationAxis;
};
