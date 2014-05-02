#include "PostProcess.h"


#include <gltools/misc.h>



PostProcess::PostProcess() : Element()
{
	m_glare = false;
	m_hdr = false;
	m_colorGrading = false;
	m_vignette = false;

	// GLARE ===================
	m_glareNumBlurIterations = 4;
	m_glareAmount = 1.0f;
	m_glareThreshold = 0.0f;

	std::string basePath = base::path("src") + "/framework/elements/postprocess/";

	base::Shader::ShaderSourcePtr dsbpssrc = base::Shader::ShaderSource::create();
	dsbpssrc->define( "DO_BRIGHTNESS_PASS" );
	dsbpssrc->file( basePath + "PostProcess.downsample.ps.glsl" );
	m_downsampleBrightnessPassShader = base::Shader::create("PostProcess.downsampleBrightness").attachVSFromFile( basePath + "PostProcess.vs.glsl" ).attachPS(dsbpssrc);

	m_downsampleShader = base::Shader::loadFromFile( basePath + "PostProcess.vs.glsl", basePath + "PostProcess.downsample.ps.glsl", "PostProcess.downsample" );
	m_downsampleShader2= base::Shader::loadFromFile( basePath + "PostProcess.vs.glsl", basePath + "PostProcess.downsample2.ps.glsl", "PostProcess.downsample2" );

	base::Shader::ShaderSourcePtr hblurSrc = base::Shader::ShaderSource::create();
	hblurSrc->verbatim( "#define HORIZONTAL_BLUR_5" );
	hblurSrc->file( basePath + "PostProcess.blur.ps.glsl" );
	m_horizontalBlurShader = base::Shader::create().attachVSFromFile( basePath + "PostProcess.vs.glsl" ).attachPS(hblurSrc);
	m_horizontalBlurShader->setUniform("sigma", 2.5f);

	base::Shader::ShaderSourcePtr vblurSrc = base::Shader::ShaderSource::create();
	vblurSrc->verbatim( "#define VERTICAL_BLUR_5" );
	vblurSrc->file( basePath + "PostProcess.blur.ps.glsl" );
	m_verticalBlurShader = base::Shader::create().attachVSFromFile( basePath + "PostProcess.vs.glsl" ).attachPS(vblurSrc);
	m_verticalBlurShader->setUniform("sigma", 2.5f);

	m_inputBlurredUniformArray = base::Attribute::createSampler2d();

	// HDR ===================
	m_hdrTreatment = HDR_LINEAR_TO_SRGB;
	FilmicParameters p;
	p.whitePoint = 11.2f;
	p.A = 0.22f; // shoulder strength
	p.B = 0.30f; // linear strength
	p.C = 0.10f; // linear angle
	p.D = 0.20f; // toe strength
	p.E = 0.01f; // toe numerator
	p.F = 0.30f; // toe denominator
	m_parmsFilmic = p;

	m_exposure = 1.0f;

	// COLORGRADING ==============================
	m_colorgradeShadows = math::Vec3f( .5f );
	m_colorgradeMidtones = math::Vec3f( 0.5f );
	m_colorgradeHighlights = math::Vec3f( 0.5f );
	m_colorgradeSaturation = 1.0f;

	// VIGNETTE ==============================
	m_vignetteStrength = 1.0f;
	m_vignetteScale= 1.0f;
	m_vignetteSoftness= 0.0f;

	// now setup final pixelshader
	m_psSource = base::Shader::ShaderSource::create();
	m_psSource->file(basePath + "PostProcess.ps.glsl");
	m_ps = base::Shader::create("postprocess").attachVSFromFile( basePath + "PostProcess.vs.glsl" ).attachPS(m_psSource);



	// create input texture and FBO---
	int width = base::Context::getCurrentContext()->getViewportWidth();
	int height = base::Context::getCurrentContext()->getViewportHeight();

	m_inputTexture = base::Texture2d::createRGBAFloat32( width, height );
	m_inputFBO = base::FBO::create().width(width).height(height).attach(m_inputTexture);

	setupShader();
}

PostProcess::Ptr PostProcess::create()
{
	return std::make_shared<PostProcess>();
}

void PostProcess::setupShader()
{
	// if there is no input specified, we will not setup the shader
	// (it will be done when the input is been set)
	if(!m_inputTexture)
		return;

	m_ps->setUniform( "input", m_inputTexture->getUniform() );

	// GLARE ================================
	if( m_glare )
	{
		m_psSource->define( "DO_GLARE" );
		m_psSource->define( "GLARE_NUM_BLUR_ITERATIONS", base::toString<int>(std::max( 1, m_glareNumBlurIterations)));

		m_inputBlurredUniformArray->resize(m_glareNumBlurIterations);

		// setup blur fbos and textures ---
		m_brightnessFBO = base::FBO::create().depthBuffer(false).attach(base::Texture2d::createRGBAFloat32(m_inputTexture->width()/2, m_inputTexture->height()/2));
		m_downsampleBrightnessPassShader->setUniform( "input", m_inputTexture->getUniform() );

		m_bloomDownsampleFBOs.resize(std::max(m_glareNumBlurIterations-1,0));
		m_bloomDownsampleOut.resize(std::max(m_glareNumBlurIterations-1,0));

		m_bloomHBlurFBOs.resize(m_glareNumBlurIterations);
		m_bloomHBlurOut.resize(m_glareNumBlurIterations);
		m_bloomVBlurFBOs.resize(m_glareNumBlurIterations);
		m_bloomVBlurOut.resize(m_glareNumBlurIterations);
		base::Texture2dPtr prev = m_brightnessFBO->getAttachedTexture2d(0);
		for( int i=0;i<m_glareNumBlurIterations;++i )
		{
			int downSamplePass = i-1;
			if(downSamplePass>=0)
			{
				m_bloomDownsampleOut[downSamplePass] = base::Texture2d::createRGBAFloat32(prev->width()/2, prev->height()/2);
				m_bloomDownsampleFBOs[downSamplePass] = base::FBO::create().depthBuffer(false).attach(m_bloomDownsampleOut[downSamplePass]);
				prev = m_bloomDownsampleOut[downSamplePass];
			}


			m_bloomHBlurOut[i] = base::Texture2d::createRGBAFloat32(prev->width(), prev->height());
			m_bloomHBlurFBOs[i] = base::FBO::create().depthBuffer(false).attach(m_bloomHBlurOut[i]);

			m_bloomVBlurOut[i] = base::Texture2d::createRGBAFloat32(prev->width(), prev->height());
			m_bloomVBlurFBOs[i] = base::FBO::create().depthBuffer(false).attach(m_bloomVBlurOut[i]);

			m_inputBlurredUniformArray->set<int>(i, m_bloomVBlurOut[i]->m_id );
		}

		m_ps->setUniform( "inputBlurred", m_inputBlurredUniformArray );


		std::string sumCode = "vec3(0.0)";
		for( int i=0;i<m_glareNumBlurIterations;++i )
			sumCode += " + texture2D( inputBlurred[" + base::toString<int>(i) + "], uv ).xyz";

		// debug
		//sumCode = "texture2D( inputBlurred[0], uv ).xyz";

		m_psSource->define( "GLARE_SUM_BLURRED", sumCode);

		setGlareAmount(m_glareAmount);
		setGlareThreshold( m_glareThreshold );

			
	}else
		m_psSource->undefine( "DO_GLARE" );

	// HDR =================================
	if( m_hdr )
	{
		setExposure(m_exposure);

		switch( m_hdrTreatment )
		{
		case HDR_LINEAR_TO_SRGB:
			{
				m_psSource->define( "DO_HDR_LINEAR_TO_SRGB" );
			}break;
		case HDR_FILMIC_TONEMAPPING:
			{
				m_psSource->define( "DO_HDR_FILMICTONEMAPPING" );
				setFilmicParms(m_parmsFilmic);
			}break;
		case HDR_NONE:
		default:
			break;
		};
	}else
	{
		m_psSource->undefine( "DO_HDR_FILMICTONEMAPPING" );
		m_psSource->undefine( "DO_HDR_LINEAR_TO_SRGB" );
	}

	// COLORGRADING ==================================
	if( m_colorGrading )
	{
		m_psSource->define( "DO_COLORGRADING" );
		setColorGradingShadows( m_colorgradeShadows );
		setColorGradingMidtones( m_colorgradeMidtones );
		setColorGradingHighlights( m_colorgradeHighlights );
		setColorGradingSaturation( m_colorgradeSaturation );
	}else
		m_psSource->undefine( "DO_COLORGRADING" );

	// VIGNETTE ===================================
	if( m_vignette )
	{
		m_psSource->define( "DO_VIGNETTE" );
		setVignetteStrength( m_vignetteStrength );
		setVignetteScale( m_vignetteScale );
		setVignetteSoftness( m_vignetteSoftness );
	}else
		m_psSource->undefine( "DO_VIGNETTE" );

	// reload
	m_ps->reload();
}


void PostProcess::setInput( base::Texture2dPtr input )
{
	if( m_inputTexture != input )
	{
		m_inputTexture = input;
		setupShader();
	}
}

void PostProcess::begin(base::Context::Ptr context, float time)
{
	m_inputFBO->begin();
}
void PostProcess::end(base::Context::Ptr context, float time)
{
	m_inputFBO->end();

	glDisable(GL_DEPTH_TEST);
	///*
	if( m_glare )
	{
		// brightness pass
		m_brightnessFBO->begin();
		m_downsampleBrightnessPassShader->setUniform( "input", m_inputTexture->getUniform() );
		m_downsampleBrightnessPassShader->setUniform( "invInputRes", 1.0f/m_brightnessFBO->getAttachedTexture2d(0)->width(), 1.0f/m_brightnessFBO->getAttachedTexture2d(0)->height() );
		context->renderScreen(m_downsampleBrightnessPassShader);
		m_brightnessFBO->end();


		// blur
		base::Texture2dPtr prevOut = m_brightnessFBO->getAttachedTexture2d(0);
		for( int i=0;i<m_glareNumBlurIterations;++i )
		{
			int downSamplePass = i-1;
			if(downSamplePass>=0)
			{
				m_downsampleShader->setUniform( "input", prevOut->getUniform() );
				m_downsampleShader->setUniform( "invInputRes", 1.0f/m_bloomDownsampleOut[downSamplePass]->width(), 1.0f/m_bloomDownsampleOut[downSamplePass]->height() );

				m_bloomDownsampleFBOs[downSamplePass]->begin();
				context->renderScreen(m_downsampleShader);
				m_bloomDownsampleFBOs[downSamplePass]->end();
				prevOut = m_bloomDownsampleOut[downSamplePass];
			}

			m_bloomHBlurFBOs[i]->begin();
			m_horizontalBlurShader->setUniform( "input", prevOut->getUniform() );
			m_horizontalBlurShader->setUniform( "blurSize", 1.0f/(float)prevOut->width() );
			context->renderScreen(m_horizontalBlurShader);
			m_bloomHBlurFBOs[i]->end();

			m_bloomVBlurFBOs[i]->begin();
			m_verticalBlurShader->setUniform( "input", m_bloomHBlurOut[i]->getUniform() );
			m_verticalBlurShader->setUniform( "blurSize", 1.0f/(float)m_bloomHBlurOut[i]->height() );
			context->renderScreen(m_verticalBlurShader);
			m_bloomVBlurFBOs[i]->end();
		}
	}

	context->renderScreen(m_ps);
	//context->renderScreen(m_bloomDownsampleOut[0]);
	//*/
}


// GLARE =============================================

void PostProcess::setGlareEnabled( bool enabled )
{
	m_glare = enabled;
	setupShader();
}

void PostProcess::setGlareAmount( float amount )
{
	m_glareAmount = amount;
	m_ps->setUniform( "glareAmount", m_glareAmount );
}

float PostProcess::getGlareAmount() const
{
	return m_glareAmount;
}

bool PostProcess::glareIsEnabled()const
{
	return m_glare;
}

void PostProcess::setGlareThreshold( float threshold )
{
	m_glareThreshold = threshold;
	m_downsampleBrightnessPassShader->setUniform( "threshold", m_glareThreshold );
}

float PostProcess::getGlareThreshold()const
{
	return m_glareThreshold;
}

void PostProcess::setGlareBlurIterations( int numBlurIterations )
{
	m_glareNumBlurIterations = numBlurIterations;
	setupShader();
}

int PostProcess::getGlareBlurIterations()const
{
	return m_glareNumBlurIterations;
}

// HDR =============================================

void PostProcess::setHDREnabled( bool enabled )
{
	m_hdr = enabled;
	setupShader();
}

bool PostProcess::hdrIsEnabled()const
{
	return m_hdr;
}

PostProcess::FilmicParameters PostProcess::getFilmicParms()const
{
	return m_parmsFilmic;
}

void PostProcess::setFilmicParms(FilmicParameters parms)
{
	m_parmsFilmic = parms;
	m_ps->setUniform( "A", m_parmsFilmic.A );
	m_ps->setUniform( "B", m_parmsFilmic.B );
	m_ps->setUniform( "C", m_parmsFilmic.C );
	m_ps->setUniform( "D", m_parmsFilmic.D );
	m_ps->setUniform( "E", m_parmsFilmic.E );
	m_ps->setUniform( "F", m_parmsFilmic.F );
	m_ps->setUniform( "linearWhitePoint", m_parmsFilmic.whitePoint );
}

float PostProcess::getExposure()const
{
	return m_exposure;
}
void PostProcess::setExposure( float e )
{
	m_exposure = e;
	m_ps->setUniform( "exposure", e );
}


// COLOR GRADING ==========================================
void PostProcess::setColorGradingEnabled( bool enabled )
{
	m_colorGrading = enabled;
	setupShader();
}

bool PostProcess::colorGradingIsEnabled()const
{
	return m_colorGrading;
}

void PostProcess::resetColorGrading()
{
	setColorGradingShadows( math::Vec3f( .5f ) );
	setColorGradingMidtones( math::Vec3f( 0.5f ) );
	setColorGradingHighlights( math::Vec3f( 0.5f ) );
	setColorGradingSaturation( 1.0f );
}

void PostProcess::setColorGradingShadows( const math::Vec3f shadows )
{
	m_colorgradeShadows = shadows;
	m_ps->setUniform( "gradeShadows", m_colorgradeShadows );
}

math::Vec3f PostProcess::getColorGradingShadows()const
{
	return m_colorgradeShadows;
}

void PostProcess::setColorGradingMidtones( const math::Vec3f midtones )
{
	m_colorgradeMidtones = midtones;
	m_ps->setUniform( "gradeMidtones", m_colorgradeMidtones );
}

math::Vec3f PostProcess::getColorGradingMidtones()const
{
	return m_colorgradeMidtones;
}

void PostProcess::setColorGradingHighlights( const math::Vec3f highlights )
{
	m_colorgradeHighlights = highlights;
	m_ps->setUniform( "gradeHighlights", m_colorgradeHighlights );
}

math::Vec3f PostProcess::getColorGradingHighlights()const
{
	return m_colorgradeHighlights;
}

void PostProcess::setColorGradingSaturation( float saturation )
{
	m_colorgradeSaturation = saturation;
	m_ps->setUniform( "gradeSaturation", m_colorgradeSaturation );
}

float PostProcess::getColorGradingSaturation()const
{
	return m_colorgradeSaturation;
}



// VIGNETTE ==========================================
void PostProcess::setVignetteEnabled( bool enabled )
{
	m_vignette = enabled;
	setupShader();
}

bool PostProcess::isVignetteEnabled()const
{
	return m_vignette;
}

void PostProcess::setVignetteStrength( float vignetteStrength )
{
	m_vignetteStrength = vignetteStrength;
	m_ps->setUniform( "vignetteStrength", m_vignetteStrength );

}

float PostProcess::getVignetteStrength()const
{
	return m_vignetteStrength;
}

void PostProcess::setVignetteScale( float scale )
{
	m_vignetteScale = scale;
	m_ps->setUniform( "vignetteScale", m_vignetteScale );
}

float PostProcess::getVignetteScale()const
{
	return m_vignetteScale;
}

void PostProcess::setVignetteSoftness( float softness )
{
	m_vignetteSoftness = softness;
	m_ps->setUniform( "vignetteSoftness", m_vignetteSoftness );
}

float PostProcess::getVignetteSoftness()const
{
	return m_vignetteSoftness;
}
