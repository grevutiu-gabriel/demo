#pragma once
#include <util/shared_ptr.h>
#include <util/StringManip.h>
#include <gfx/Shader.h>
#include <gfx/Texture.h>
#include <gfx/Context.h>
#include <gfx/FBO.h>

#include "../../Element.h"

struct PostProcess : public Element
{
	typedef std::shared_ptr<PostProcess> Ptr;

	// parameters for filmic tonemapping
	struct FilmicParameters
	{
		float whitePoint, A, B, C, D, E, F;
	};

	enum HDRTreatment
	{
		HDR_NONE = 0,
		HDR_LINEAR_TO_SRGB = 1,
		HDR_FILMIC_TONEMAPPING = 2
	};

	PostProcess();

	static Ptr              create();

	// onherited from Element
	virtual void            begin(base::Context::Ptr context)override;
	virtual void            end(base::Context::Ptr context)override;
	virtual void            render(base::Context::Ptr context, float time)override;

	void                    setupShader();
	void                    setInput( base::Texture2dPtr input );


	// GLARE =================
	void                                      setGlareEnabled( bool enabled );
	void                                       setGlareAmount( float amount );
	float                                              getGlareAmount() const;
	bool                                                glareIsEnabled()const;
	void                                 setGlareThreshold( float threshold );
	float                                            getGlareThreshold()const;
	void                      setGlareBlurIterations( int numBlurIterations );
	int                                         getGlareBlurIterations()const;

	// HDR ===================
	void                                        setHDREnabled( bool enabled );
	bool                                                  hdrIsEnabled()const;
	FilmicParameters                                    getFilmicParms()const;
	void                               setFilmicParms(FilmicParameters parms);
	float                                                  getExposure()const;
	void                                               setExposure( float e );

	// COLOR GRADING ==================
	void                               setColorGradingEnabled( bool enabled );
	void                                                  resetColorGrading();
	bool                                         colorGradingIsEnabled()const;
	void                  setColorGradingShadows( const math::Vec3f shadows );
	math::Vec3f                                 getColorGradingShadows()const;
	void                setColorGradingMidtones( const math::Vec3f midtones );
	math::Vec3f                                getColorGradingMidtones()const;
	void            setColorGradingHighlights( const math::Vec3f highlights );
	math::Vec3f                              getColorGradingHighlights()const;
	float                                    getColorGradingSaturation()const;
	void                        setColorGradingSaturation( float saturation );

	// VIGNETTE ========================
	void                                   setVignetteEnabled( bool enabled );
	bool                                             isVignetteEnabled()const;
	void                        setVignetteStrength( float vignetteStrength );
	float                                          getVignetteStrength()const;
	void                                      setVignetteScale( float scale );
	float                                             getVignetteScale()const;
	void                                setVignetteSoftness( float softness );
	float                                          getVignetteSoftness()const;



private:
	// members
	base::Texture2dPtr            m_inputTexture;
	base::FBO::Ptr                m_inputFBO;

	// GLARE =====================
	bool                                                m_glare;
	float                                         m_glareAmount;
	float                                      m_glareThreshold;
	int                                m_glareNumBlurIterations;


	base::FBOPtr                                m_brightnessFBO;
	std::vector<base::FBOPtr>             m_bloomDownsampleFBOs;
	std::vector<base::Texture2dPtr>        m_bloomDownsampleOut;
	std::vector<base::FBOPtr>                  m_bloomHBlurFBOs;
	std::vector<base::Texture2dPtr>             m_bloomHBlurOut;
	std::vector<base::FBOPtr>                  m_bloomVBlurFBOs;
	std::vector<base::Texture2dPtr>             m_bloomVBlurOut;

	base::AttributePtr               m_inputBlurredUniformArray;

	base::Shader::Ptr            m_downsampleBrightnessPassShader; // used for brightness pass
	base::Shader::Ptr            m_downsampleShader;
	base::Shader::Ptr            m_downsampleShader2;
	base::Shader::Ptr            m_horizontalBlurShader;
	base::Shader::Ptr            m_verticalBlurShader;

	// HDR =====================
	bool                                                  m_hdr;
	HDRTreatment                                 m_hdrTreatment;
	FilmicParameters                              m_parmsFilmic; // parameters for filmic tonemapping
	float                                            m_exposure;

	// COLOR GRADING ===================
	bool                                         m_colorGrading;
	math::Vec3f                             m_colorgradeShadows;
	math::Vec3f                            m_colorgradeMidtones;
	math::Vec3f                          m_colorgradeHighlights;
	float                                m_colorgradeSaturation;

	// VIGNETTE =======================
	bool                                             m_vignette;
	float                                    m_vignetteStrength;
	float                                       m_vignetteScale;
	float                                    m_vignetteSoftness;

	// main shader
	base::Shader::ShaderSourcePtr        m_psSource; // we keep the source because options will change defines
	base::Shader::Ptr                    m_ps;
};
