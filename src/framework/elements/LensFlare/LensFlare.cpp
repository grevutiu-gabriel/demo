#include "LensFlare.h"

#include <gfx/Geometry.h>
#include <util/fs.h>


LensFlare::LensFlare() : Element()
{
	m_lightpos = base::Attribute::createV3f();
	m_lightpos->appendElement(1.0f, 0.0f, 0.0f);
	m_ring = ChromaticRing::create(m_lightpos);

	m_lights.push_back( math::V3f(0.0f, 1.0f, 0.0f) );

	addProperty( "lights", &m_lights );
}


LensFlare::Ptr LensFlare::create()
{
	return std::make_shared<LensFlare>();
}

void LensFlare::render(base::Context::Ptr context, float time)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable( GL_DEPTH_TEST );
	glDisable( GL_CULL_FACE );

	m_ring->render(context);

	std::cout << "numLights:" << m_lights.size() << std::endl;
	for( auto vec:m_lights )
	{
		std::cout << "\t" << vec.x << " " << vec.y << " " << vec.z << std::endl;
	}

}

void LensFlare::setLightPos(const math::V3f &pos)
{
	m_lightpos->set<math::V3f>(0, pos);
}

void LensFlare::serialize(Serializer &out)
{
	Element::serialize(out);

	// lights
	{
		houdini::json::ArrayPtr jsonLights = houdini::json::Array::create();
		for(auto light:m_lights)
		{
			houdini::json::ArrayPtr jsonLight = houdini::json::Array::create();
			jsonLight->appendValue<float>(light.x);
			jsonLight->appendValue<float>(light.y);
			jsonLight->appendValue<float>(light.z);
			jsonLights->append(jsonLight);
		}
		out.write("lights", jsonLights);
	}
}

void LensFlare::deserialize(Deserializer &in)
{
	Element::deserialize(in);

	// lights
	{
		m_lights.clear();
		houdini::json::ArrayPtr jsonLights = in.readArray("lights");
		for( int i=0, numElements=int(jsonLights->size());i<numElements;++i )
		{
			houdini::json::ArrayPtr jsonLight = jsonLights->getArray(i);
			math::V3f light;
			light.x = jsonLight->get<float>(0);
			light.y = jsonLight->get<float>(1);
			light.z = jsonLight->get<float>(2);
			m_lights.push_back(light);
		}
	}
}


ChromaticRing::ChromaticRing(base::Attribute::Ptr lightpos):
	m_thickness(0.1f),
	m_lightpos(lightpos)
{
	std::string basePath = base::path("src") + "/framework/elements/LensFlare/";
	std::string basePathData = base::path("data") + "/framework/elements/LensFlare/";

	m_position = base::Attribute::createV2f();
	m_position->appendElement( 1.0f, 1.0f );
	m_scale = base::Attribute::createFloat();
	m_scale->appendElement( 1.0f );

	m_geoComplexity = 160;
	m_thickness = 0.3f;
	m_brightness = 0.1f;
	m_spanAngle = math::degToRad(120.0f);
	m_falloffSpanAngle = math::degToRad(30.0f);
	m_perpectiveCorrectionFactor = 4.0f;

	base::Attribute::Ptr positions = base::Attribute::createV3f();
	base::Attribute::Ptr uvs = base::Attribute::createV2f();
	m_geometryTriangles = std::make_shared<base::Geometry>(base::Geometry::TRIANGLE);
	m_geometryTriangles->setAttr( "P", positions);
	m_geometryTriangles->setAttr( "UV", uvs);
	m_geometry = std::make_shared<base::Geometry>(base::Geometry::LINE);
	m_geometry->setAttr( "P", positions);
	m_geometry->setAttr( "UV", uvs);



	m_gradientTexture = base::Texture2d::load( basePathData + "ghost_a_multicolor.png", GL_RGBA8 );
	m_fadeUTexture = base::Texture1d::createFloat32();

	m_shader = base::Shader::loadFromFile( basePath + "ChromaticRing" );
	m_shader->setUniform( "mvpm", math::Matrix44f::Identity() );
	m_shader->setUniform( "lpos", lightpos );
	m_shader->setUniform( "position", m_position );
	m_shader->setUniform( "scale", m_scale );
	m_shader->setUniform( "brightness", m_brightness );
	m_shader->setUniform( "gradientTex", m_gradientTexture );
	m_shader->setUniform( "fadeUTex", m_fadeUTexture->getUniform() );
	m_shader->setUniform( "perpectiveCorrectionFactor", m_perpectiveCorrectionFactor );

	updateGeometry();
}

ChromaticRing::Ptr ChromaticRing::create(base::Attribute::Ptr lightpos)
{
	return std::make_shared<ChromaticRing>(lightpos);
}

void ChromaticRing::render(base::Context::Ptr context)
{
	//context->render( m_geometry, m_shader );
	glEnable( GL_BLEND );
	glBlendFunc(GL_ONE, GL_ONE);
	m_shader->setUniform("brightness", m_brightness);
	context->render( m_geometryTriangles, m_shader );
	m_shader->setUniform("brightness", m_brightness*5.0f);
	context->render( m_geometry, m_shader );
	glDisable(GL_BLEND);
}

void ChromaticRing::updateGeometry()
{
	m_geometry->clear();

	base::Attribute::Ptr positions = m_geometry->getAttr("P");
	base::Attribute::Ptr uvs = m_geometry->getAttr("UV");

	int uSubdivisions = m_geoComplexity;


	float dPhi = MATH_PIf/uSubdivisions;
	float dU = 0.5f/uSubdivisions;
	float phi;

	float scale = 0.5f;

	{
		float ratio = 0.5f;
		float ratio2 = 0.5f;
		float dPhiSplit[2];
		dPhiSplit[0] = dPhi*ratio;
		dPhiSplit[1] = dPhi*(1.0f-ratio);
		float dUSplit[2];
		dUSplit[0] = dU*ratio2;
		dUSplit[1] = dU*(1.0f-ratio2);

		math::V3f p,p1,p2;
		// x-z
		phi = dPhi*0.5f*scale;
		float u=0.5f + dU*0.5f;
		for( int j = 0; j<(uSubdivisions); ++j  )
		{
			float phiJitter = (math::g_randomNumber.randomFloat()-0.5f)*dPhi;
			float phiJitter2 = (math::g_randomNumber.randomFloat()-0.5f)*dPhi;
			math::V3f p;
			p.x = cos((phi+phiJitter)*scale);
			p.y = sin((phi+phiJitter2)*scale);
			p.z = 0.0f;

			p1 = p;
			p2 = p*(1.0f-m_thickness);

			positions->appendElement( p1 );
			uvs->appendElement( u, 1.0f );
			positions->appendElement( p2 );
			uvs->appendElement( u, 0.0f );

			phi+=dPhi;
			u+=dU;
			//phi+=dPhiSplit[j%2];
			//u+=dUSplit[j%2];
		}
		phi-=MATH_2PIf;
		u-=1.0f;
		for( int j = 0; j<(uSubdivisions); ++j  )
		{
			float phiJitter = (math::g_randomNumber.randomFloat()-0.5f)*dPhi;
			float phiJitter2 = (math::g_randomNumber.randomFloat()-0.5f)*dPhi;
			math::V3f p;
			p.x = cos((phi+phiJitter)*scale);
			p.y = sin((phi+phiJitter2)*scale);
			p.z = 0.0f;

			p1 = p;
			p2 = p*(1.0f-m_thickness);

			positions->appendElement( p1 );
			uvs->appendElement( u, 1.0f );
			positions->appendElement( p2 );
			uvs->appendElement( u, 0.0f );

			phi+=dPhi;
			u+=dU;
			//phi+=dPhiSplit[j%2];
			//u+=dUSplit[j%2];
		}
	}

	int j;
	for( j = 0; j<(uSubdivisions*2)-1; ++j  )
	{
		m_geometry->addLine( j*2, (j*2)+1 );
		m_geometry->addLine( (j*2)+1, (j*2)+2 );
		m_geometryTriangles->addTriangle( j*2, (j*2)+1, (j*2)+2 );
		m_geometryTriangles->addTriangle( (j*2)+2, (j*2)+1, (j*2)+3 );
	}
	m_geometry->addLine( j*2, j*2+1);
	m_geometry->addLine( j*2+1, 0 );
	m_geometryTriangles->addTriangle( j*2, (j*2)+1, 0 );
	m_geometryTriangles->addTriangle( 0, (j*2)+1, 1 );


	// build fade off texture
	{
		int res = 128;
		int resh = res/2;
		// normalize span angle
		float span = (m_spanAngle/MATH_2PIf)*0.5f+0.5f;
		float spanFalloff = span + (m_falloffSpanAngle/MATH_2PIf);
		std::cout << "span: " << span << " " << spanFalloff << std::endl;

		std::vector<float> samples;
		for(int i=0;i<resh;++i)
		{
			float t = (1.0f - float(i)/(resh-1))*0.5f + 0.5f;
			float sample = 1.0f - math::smoothstep( span, spanFalloff, t );
			samples.push_back(sample);
		}
		std::vector<float> samples2(samples.rbegin(), samples.rend());
		samples.insert( samples.end(), samples2.begin(), samples2.end() );
		m_fadeUTexture->uploadFloat32( res, &samples[0] );
	}
}



LensFlare::~LensFlare()
{

}

REGISTERCLASS2( LensFlare, Element )
