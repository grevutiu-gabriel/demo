
#include "Stars.h"

float decodeSNORM( sint32 x )
{
	const sint32 n = 16;
	return std::min( 1.0f, std::max( -1.0f, (float)x/(powf(2.0,n-1)-1)));
}

Stars::Stars() : Element()
{
	std::string basePathData = base::path("data") + "/framework/elements/stars/";
	std::string basePath = base::path("src") + "/framework/elements/stars/";

	// defaults ---
	m_starBrightnessScale = 10.0f;
	m_domeBrightnessScale = 0.05f;
	m_motionblurStrength = 1.0f;

	m_fixedRotation = false;
	m_fixedRotationAngle = math::degToRad(5.0f);
	m_fixedRotationAxis = math::Vec3f(1.0f, 1.0f, 0.0f).normalized();


	// STARS ====================================================
	int numStars = 9110;
	base::fs::File *f = base::fs::open( basePathData + "stars.3S16_3F16" );
	int starDataSize = numStars * 12;
	unsigned char *starData = (unsigned char*)malloc(starDataSize);
	unsigned char *ptr = starData;
	base::fs::read( f, starData, starDataSize, 1 );

	m_stars = base::Geometry::createPointGeometry();
	base::AttributePtr pAttr = m_stars->getAttr("P");
	base::AttributePtr cAttr = base::Attribute::createV3f();
	m_stars->setAttr( "Cd", cAttr );

	for( int i=0;i<numStars;++i )
	{
		float x = decodeSNORM( ((sint16* )ptr)[0] );
		float y = decodeSNORM( ((sint16* )ptr)[1] );
		float z = decodeSNORM( ((sint16* )ptr)[2] );
		ptr += 6;

		float r = ((half* )ptr)[0];
		float g = ((half* )ptr)[1];
		float b = ((half* )ptr)[2];
		ptr += 6;


		m_stars->addPoint( pAttr->appendElement( x, y, z ));
		cAttr->appendElement( r, g, b );
	}

	m_starsShader = base::Shader::loadFromFile( basePath + "Stars.stars" );

	base::fs::close(f);


	// aa lut
	m_aaTex = base::Texture1d::createR8(32);
	unsigned char const m_aaTexels[32] = { // used for antialiasing points
			255, 253, 250, 246, 241, 234, 226, 216,
			203, 189, 173, 156, 138, 120, 102,  85,
				70,  56,  43,  32,  24,  18,  14,  11,
				8,   6,   4,   3,   2,   1,   0,   0
	};
	m_aaTex->uploadR8(32, m_aaTexels);


	m_starsShader->setUniform( "aaTex", m_aaTex->getUniform());
	m_starsShader->setUniform( "mvm0R", math::Matrix44f::Identity());

	m_currentFov = 0.0f;
	m_currentScreenHeight = 0;
	m_starsShader->setUniform( "starRadius", 0.004f);
	m_starsShader->setUniform( "starLuminanceScale", 1.0f );
	m_starsShader->setUniform( "starBrightnessScale", m_starBrightnessScale );



	// DOME ============================================
	m_dome = base::Geometry::createSphere(30, 30, 0.5);
	m_dome->addNormals();
	m_domeShader = base::Shader::loadFromFile( basePath + "Stars.dome" );

	base::ImagePtr domeImage = base::Image::load( basePathData + "milky_way_texture_cube.png");
	int width = domeImage->m_width;
	int height = 512;

	base::ImagePtr faces[6];
	faces[0] = domeImage->copy( 0, height*0, width, height );
	faces[1] = domeImage->copy( 0, height*1, width, height );
	faces[2] = domeImage->copy( 0, height*2, width, height );
	faces[3] = domeImage->copy( 0, height*3, width, height );
	faces[4] = domeImage->copy( 0, height*4, width, height );
	faces[5] = domeImage->copy( 0, height*5, width, height );

	domeCubeMap = base::TextureCube::createRGBA8();
	domeCubeMap->upload( faces );

	m_domeShader->setUniform( "domeTex", domeCubeMap->getUniform() );
	m_domeShader->setUniform( "domeBrightnessScale", m_domeBrightnessScale );
}

Stars::Ptr Stars::create()
{
	return std::make_shared<Stars>();
}

void Stars::render( base::Context::Ptr context, float time )
{
	// radius and star luminance scale depend on screen resolution and fov
	//TODO: get hold of fov value..currently we use hardcoded stuff
	float camfov = math::degToRad(50.0f);
	if( (camfov !=m_currentFov)||(context->getViewportHeight()!=m_currentScreenHeight) )
	{
		m_currentFov = camfov;
		m_currentScreenHeight = context->getViewportHeight();

		// compute radius such that a star takes up 2.8 pixels on screen
		float r = 1.4f*( tan(m_currentFov*0.5f)/(m_currentScreenHeight*0.5f) );
		m_starsShader->setUniform( "starRadius", r );

		// luminance scale
		float viewFactor = tan(m_currentFov);
		float resolutionFactor = m_currentScreenHeight / 1920.0f;
		float starLuminanceScale = 1.0f / 256.0f / (viewFactor * viewFactor) * (resolutionFactor * resolutionFactor);
		m_starsShader->setUniform( "starLuminanceScale", starLuminanceScale );
	}
	

	base::Context::TransformState ts;
	context->getTransformState(ts);

	math::Matrix44f mvm = ts.modelViewMatrix.getOrientation();
	context->setViewMatrix( mvm );

	// this rotation descripes the rotation from mvm*p to mvm0*p
	math::Vec3f axis;
	float angle;

	if( m_fixedRotation )
	{
		// use user specified rotation axis and angle...makes for some nice star trail shots
		axis = m_fixedRotationAxis;
		angle = m_fixedRotationAngle;
	}else
	{
		// derive axis and angle of rotation from mvm to mvm0
		math::extractAxisAngle( m_mvm0*mvm.inverted(), axis, angle );
	}

	// reconstruct matrix and amplify angle
	math::Matrix44f mvm0R = math::Matrix44f::RotationMatrix( axis, math::clamp( angle*m_motionblurStrength, 0.0f, MATH_PIf ) );

	m_starsShader->setUniform( "mvm0R", mvm0R );

	glDisable( GL_CULL_FACE );
	glDisable( GL_DEPTH_TEST );
	glEnable( GL_BLEND );
	glBlendFunc(GL_ONE, GL_ONE);
	context->render( m_dome, m_domeShader );
	context->render( m_stars, m_starsShader );
	glDisable( GL_BLEND );
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_CULL_FACE );


	m_mvm0 = mvm;
	context->setTransformState( ts );
}


void Stars::setStarBrightnessScale( float starBrightnessScale )
{
	m_starBrightnessScale = starBrightnessScale;
	m_starsShader->setUniform( "starBrightnessScale", m_starBrightnessScale );
}

float Stars::getStarBrightnessScale()const
{
	return m_starBrightnessScale;
}

void Stars::setDomeBrightnessScale( float domeBrightnessScale )
{
	m_domeBrightnessScale = domeBrightnessScale;
	m_domeShader->setUniform( "domeBrightnessScale", m_domeBrightnessScale );
}

float Stars::getDomeBrightnessScale()const
{
	return m_domeBrightnessScale;
}


void Stars::setFixedRotationEnabled( bool enabled )
{
	m_fixedRotation = enabled;
}

bool Stars::isFixedRotationEnabled()const
{
	return m_fixedRotation;
}

void Stars::setMotionblurScale( float mbscale )
{
	m_motionblurStrength = mbscale;
}

float Stars::getMotionblurScale()const
{
	return m_motionblurStrength;
}

// rotationAngle in rad
void Stars::setFixedRotation( const math::Vec3f &rotationAxis, const float &rotationAngle )
{
	m_fixedRotationAxis = rotationAxis;
	m_fixedRotationAngle = rotationAngle;
}


REGISTERCLASS( Stars )


