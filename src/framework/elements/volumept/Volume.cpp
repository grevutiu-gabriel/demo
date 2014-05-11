
#include "Volume.h"
#include <iostream>
#include <fstream>
#include <sstream>

#include <util/fs.h>
#include <util/tuple.h>
#include <gfx/Field.h>
#include <framework/houdini/HouGeoIO.h>
#include <math/Matrix44Algo.h>




bool planeSegmentIntersection( const math::Vec3f& planeN, float planeDist,
							   const math::Vec3f& start, const math::Vec3f& end,
							   float& t )
{
	const math::Vec3f dir = end - start;
	const math::Vec3f pPoint = -planeN * planeDist;
	float denom = math::dotProduct( dir, planeN );
	if ( fabs(denom) < 1e-4f ) return false;
	t = math::dotProduct( planeN, pPoint - start ) / denom;
	return t >= 0.f && t <= 1.f;
}


inline void planePathIntersection( math::Vec3f &isect, size_t &nPoints, math::Vec3f planeN, float planeDist, const math::Vec3f &path0, const math::Vec3f &path1, const math::Vec3f &otherwise )
 {
	float t;
	if ( planeSegmentIntersection( planeN, planeDist, path0, path1, t ) )
	{
		isect = path0 + ( path1 - path0 ) * t;
		nPoints++;
	}
	else
	{
		isect = otherwise;
	}
 }

inline void planePathIntersection( math::Vec3f &isect, size_t &nPoints, math::Vec3f planeN, float planeDist, math::Vec3f path0, math::Vec3f path1, math::Vec3f path2, math::Vec3f path3 )
{
	float t;
	if ( planeSegmentIntersection( planeN, planeDist, path0, path1, t ) )
	{
		isect = path0 + ( path1 - path0 ) * t;
		nPoints++;
	} else if ( planeSegmentIntersection( planeN, planeDist, path1, path2, t ) )
	{
		isect = path1 + ( path2 - path1 ) * t;
		nPoints++;
	} else if ( planeSegmentIntersection( planeN, planeDist, path2, path3, t ) )
	{
		isect = path2 + ( path3 - path2 ) * t;
		nPoints++;
	};
}


// http://www.cg.informatik.uni-siegen.de/data/Publications/2005/rezksalamaVMV2005.pdf
size_t computeAABBPlaneIntersectionGeometry( const math::Vec3f& bbMin, const math::Vec3f& bbMax,
											 const math::Vec3f& planeN, float planeDist,
											 math::Vec3f *outVertices, math::Vec3f *outUVW )
{
	math::Vec3f bbPoints[ 8 ] = { math::Vec3f( bbMax.x, bbMax.y, bbMax.z ),
								math::Vec3f( bbMax.x, bbMax.y, bbMin.z ),
								math::Vec3f( bbMax.x, bbMin.y, bbMax.z ),
								math::Vec3f( bbMin.x, bbMax.y, bbMax.z ),
								math::Vec3f( bbMin.x, bbMax.y, bbMin.z ),
								math::Vec3f( bbMax.x, bbMin.y, bbMin.z ),
								math::Vec3f( bbMin.x, bbMin.y, bbMax.z ),
								math::Vec3f( bbMin.x, bbMin.y, bbMin.z ) };

	// precomputed vertex sorting by distance for each point being the closest
	static int vertexSequence[ 8 * 8 ] = { 0, 1, 2, 3, 4, 5, 6, 7,
										   1, 4, 5, 0, 3, 7, 2, 6,
										   2, 6, 0, 5, 7, 3, 1, 4,
										   3, 0, 6, 4, 1, 2, 7, 5,
										   4, 3, 7, 1, 0, 6, 5, 2,
										   5, 2, 1, 7, 6, 0, 4, 3,
										   6, 7, 3, 2, 5, 4, 0, 1,
										   7, 5, 4, 6, 2, 1, 3, 0 };

	// find closest point
	int closestVert = 0;
	float closestDist = std::numeric_limits<float>::max();
	float farthestDist = -std::numeric_limits<float>::max();
	for( int i = 0; i < 8; i++ )
	{
		const math::Vec3f& p = bbPoints[ i ];
		float dist = p.x * planeN.x + p.y * planeN.y + p.z * planeN.z + planeDist;
		if ( dist < closestDist )
		{
			closestDist = dist;
			closestVert = i;
		}
		farthestDist = std::max( farthestDist, dist );
	}

	if ( closestDist > 0 || farthestDist < 0 )
	{
		// there's no intersection with the plane
		return false;
	}

	size_t nPoints = 0;

	// bring points into the right order by using LUT
	const math::Vec3f& p0 = bbPoints[ vertexSequence[ 8 * closestVert + 0 ] ];
	const math::Vec3f& p1 = bbPoints[ vertexSequence[ 8 * closestVert + 1 ] ];
	const math::Vec3f& p2 = bbPoints[ vertexSequence[ 8 * closestVert + 2 ] ];
	const math::Vec3f& p3 = bbPoints[ vertexSequence[ 8 * closestVert + 3 ] ];
	const math::Vec3f& p4 = bbPoints[ vertexSequence[ 8 * closestVert + 4 ] ];
	const math::Vec3f& p5 = bbPoints[ vertexSequence[ 8 * closestVert + 5 ] ];
	const math::Vec3f& p6 = bbPoints[ vertexSequence[ 8 * closestVert + 6 ] ];
	const math::Vec3f& p7 = bbPoints[ vertexSequence[ 8 * closestVert + 7 ] ];

	planePathIntersection(outVertices[0], nPoints, planeN, planeDist, p0, p1, p4, p7 );   // i0
	planePathIntersection(outVertices[2], nPoints, planeN, planeDist, p0, p2, p5, p7 );   // i2
	planePathIntersection(outVertices[4], nPoints, planeN, planeDist, p0, p3, p6, p7 );   // i4
	planePathIntersection(outVertices[1], nPoints, planeN, planeDist, p1, p5, outVertices[0] ); // i1
	planePathIntersection(outVertices[3], nPoints, planeN, planeDist, p2, p6, outVertices[2] ); // i3
	planePathIntersection(outVertices[5], nPoints, planeN, planeDist, p3, p4, outVertices[4] ); // i5

	// compute texture coords
	for( size_t i = 0; i < 6; i++ )
	{
		const math::Vec3f& p = outVertices[i];
		//outUVW[i] = math::Vec3f(p.x + 0.5f, p.y + 0.5f, p.z + 0.5f);
		outUVW[i] = math::Vec3f(p.x, p.y, p.z);
	}
	return nPoints;
}

Volume::Volume() : Element()
{
	int width = 512;
	int height = 512;
	std::string basePath = base::path("src") + "/framework/elements/volumept/";

	// bound geometry --
	m_proxy = createProxyGeometry();


	// near clip geo --
	nearClipGeo = base::Geometry::createPolyGeometry();
	nearClipP = base::Attribute::createV3f(6);
	nearClipUVW = base::Attribute::createV3f(6);
	nearClipGeo->setAttr( "P", nearClipP );
	nearClipGeo->setAttr( "UVW", nearClipUVW );
	nearClipGeo->addPolygonVertex(5);
	nearClipGeo->addPolygonVertex(4);
	nearClipGeo->addPolygonVertex(3);
	nearClipGeo->addPolygonVertex(2);
	nearClipGeo->addPolygonVertex(1);
	nearClipGeo->addPolygonVertex(0);


	volumeBack = base::Texture2d::createRGBAFloat16( width, height );
	volumeFront = base::Texture2d::createRGBAFloat16( width, height );
	estimate = base::Texture2d::createRGBAFloat32( width, height );

	// setup offscreen render pass --
	volumeFrontFBO = base::FBO::create().width(width).height(height).attach(volumeFront);
	volumeBackFBO = base::FBO::create().width(width).height(height).attach(volumeBack);
	estimateFBO = base::FBO::create().width(width).height(height).clearColor(0.0f, 0.0f, 0.0f, 0.0f).attach(estimate);

	// setup resources for blur pass ---
	float blurSigma = 1.5f*1.0f;
	blurHOut = base::Texture2d::createRGBAFloat32(width, height);
	blurHFBO = base::FBO::create().depthBuffer(false).attach(blurHOut);
	blurVOut = base::Texture2d::createRGBAFloat32(width, height);
	blurVFBO = base::FBO::create().depthBuffer(false).attach(blurVOut);
	base::Shader::ShaderSourcePtr hblurSrc = base::Shader::ShaderSource::create();
	hblurSrc->verbatim( "#define HORIZONTAL_BLUR_5" );
	hblurSrc->file( basePath + "Volume.blur.ps.glsl" );
	blurHShader = base::Shader::create().attachVSFromFile( basePath + "Volume.vs.glsl" ).attachPS(hblurSrc);
	blurHShader->setUniform("sigma", blurSigma);
	blurHShader->setUniform( "blurSize", 1.0f/(float)width );
	blurHShader->setUniform("input", estimate);

	base::Shader::ShaderSourcePtr vblurSrc = base::Shader::ShaderSource::create();
	vblurSrc->verbatim( "#define VERTICAL_BLUR_5" );
	vblurSrc->file( basePath + "Volume.blur.ps.glsl" );
	blurVShader = base::Shader::create().attachVSFromFile( basePath + "Volume.vs.glsl" ).attachPS(vblurSrc);
	blurVShader->setUniform("sigma", blurSigma);
	blurVShader->setUniform( "blurSize", 1.0f/(float)height );
	blurVShader->setUniform("input", blurHOut);

//	{
//		math::V3f incrementalGaussian;
//		float sigma = 5.0f;
//		incrementalGaussian.x = 1.0 / (sqrt(2.0 * MATH_PI) * sigma);
//		incrementalGaussian.y = exp(-0.5 / (sigma * sigma));
//		incrementalGaussian.z = incrementalGaussian.y * incrementalGaussian.y;
//		std::cout << "test " << incrementalGaussian.x << std::endl;
//		incrementalGaussian.x *= incrementalGaussian.y;
//		incrementalGaussian.y *= incrementalGaussian.x;
//		std::cout << "test " << incrementalGaussian.x << std::endl;
//		incrementalGaussian.x *= incrementalGaussian.y;
//		incrementalGaussian.y *= incrementalGaussian.x;
//		std::cout << "test " << incrementalGaussian.x << std::endl;
//		incrementalGaussian.x *= incrementalGaussian.y;
//		incrementalGaussian.y *= incrementalGaussian.x;
//		std::cout << "test " << incrementalGaussian.x << std::endl;
//		incrementalGaussian.x *= incrementalGaussian.y;
//		incrementalGaussian.y *= incrementalGaussian.x;
//		std::cout << "test " << incrementalGaussian.x << std::endl;
//	}


	//
	volumeGeoShader = base::Shader::loadFromFile( basePath + "Volume.geo");

	volumeShader = base::Shader::loadFromFile( basePath + "Volume" );
	volumeShader->setUniform( "volumeFront", volumeFront->getUniform() );
	volumeShader->setUniform( "volumeBack", volumeBack->getUniform() );

	// create default density field ---
	float densityMax=-std::numeric_limits<float>::infinity();
	//m_density=base::ScalarField::create(math::V3i(50), math::Box3f(math::V3f(-4.0f), math::V3f(-1.0f)));
	m_normalizedDensity=base::ScalarField::create(math::V3i(50), math::Box3f(math::V3f(0.0f), math::V3f(1.0f)));

	// initialize default density
	for( int k = 0; k<m_normalizedDensity->m_resolution.z;++k )
		for( int j = 0; j<m_normalizedDensity->m_resolution.y;++j )
			for( int i = 0; i<m_normalizedDensity->m_resolution.x;++i )
			{
				//float t = ((float)i/(float)density->m_resolution.x);
				float t = ((float)j/(float)m_normalizedDensity->m_resolution.y);
				//float t = ((float)k/(float)density->m_resolution.z);
				//float value = t;
				//float value = 1.0f-t;
				float value = 0.5f;
				math::V3f lsP = m_normalizedDensity->voxelToLocal( math::V3f(float(i)+0.5f, float(j)+0.5f, float(k)+0.5f) );
				if( (lsP-math::V3f(0.5f)).getLength() > 0.2f )
					value = 0.0f;
				//value*=20.0f;
				m_normalizedDensity->lvalue(i,j,k) = value;
				densityMax = std::max(densityMax, value);
			}
	//std::cout << "densityMax " << densityMax << std::endl;


	m_normalizedDensityTexture = base::Texture3d::createFloat32();
	m_normalizedDensityTexture->uploadFloat32( m_normalizedDensity->m_resolution.x, m_normalizedDensity->m_resolution.y, m_normalizedDensity->m_resolution.z, m_normalizedDensity->getRawPointer() );
	volumeShader->setUniform( "normalizedDensity", m_normalizedDensityTexture->getUniform() );
	// TODO: once transfer function is introduced, use max of transferfunction
	volumeShader->setUniform( "st_max", densityMax );

	math::Matrix44f localToWorld = math::M44f();
	localToWorldAttr = base::Attribute::createM44f();
	localToWorldAttr->appendElement( m_normalizedDensity->m_localToWorld );
	//localToWorldAttr->appendElement( localToWorld );
	volumeShader->setUniform( "localToWorld", localToWorldAttr );

	worldToLocalAttr = base::Attribute::createM44f();
	worldToLocalAttr->appendElement( m_normalizedDensity->m_worldToLocal );
	volumeShader->setUniform( "worldToLocal", worldToLocalAttr );

	// transfer function ---
	m_transferFunction = std::make_shared<TransferFunction>();

	float scale = 1.0f;
	//float scale = 1.0f;

//	// manix tf
//	TransferFunction::PLF plf_manix;
//	plf_manix.addSample( -1000.0f, math::V4f(1.0f, 0.0f, 0.0f, 0.0f) );
//	plf_manix.addSample( 3095.0f, math::V4f(1.0f, 0.0f, 0.0f, 1.0f) );
//	m_transferFunction->setPLF(plf_manix);


//	TransferFunction::PLF plf_0;
//	plf_0.addSample( 0.0f, math::V4f(0.0f, 0.0f, 0.0f, 0.0f*scale) );
//	plf_0.addSample( 1.0f, math::V4f(0.0f, 0.0f, 0.0f, 1.0f*scale) );
//	TransferFunction::PLF plf_1;
//	plf_1.addSample( 0.0f, math::V4f(0.0f, 0.0f, 0.0f, 0.0f*scale) );
//	plf_1.addSample( 1.0f, math::V4f(1.0f, 1.0f, 1.0f, 1.0f*scale) );
//	m_transferFunction->setPLF(plf_1);

//	// artifix tf
//	TransferFunction::PLF plf_artifix;
//	plf_artifix.addSample( 0.0f, math::V4f(160.0f/255.0f, 160.0f/255.0f, 164.0f/255.0f, 0.0f*scale) );
//	plf_artifix.addSample( 0.266535, math::V4f(68.0f/255.0f, 0.0f/255.0f, 0.0f/255.0f, 0.0f*scale) );
//	plf_artifix.addSample( 0.310104, math::V4f(143.0f/255.0f, 104.0f/255.0f, 54.0f/255.0f, 0.059f*scale) );
//	plf_artifix.addSample( 0.453057, math::V4f(18.0f/255.0f, 9.0f/255.0f, 0.0f/255.0f, 1.0f*scale) );
//	plf_artifix.addSample( 1.0f, math::V4f(160.0f/255.0f, 160.0f/255.0f, 164.0f/255.0f, 1.0f*scale) );
//	m_transferFunction->setPLF(plf_artifix);

//	TransferFunction::PLF plf_artifix2;
//	plf_artifix2.addSample( 0.0f, math::V4f(160.0f/255.0f, 160.0f/255.0f, 164.0f/255.0f, 0.0f*scale) );
//	plf_artifix2.addSample( 0.3101, math::V4f(68.0f/255.0f, 0.0f/255.0f, 0.0f/255.0f, 0.0f*scale) );
//	plf_artifix2.addSample( 0.310104, math::V4f(143.0f/255.0f, 104.0f/255.0f, 54.0f/255.0f, 0.059f*scale) );
//	plf_artifix2.addSample( 0.453057, math::V4f(18.0f/255.0f, 9.0f/255.0f, 0.0f/255.0f, 1.0f*scale) );
//	plf_artifix2.addSample( 1.0f, math::V4f(160.0f/255.0f, 160.0f/255.0f, 164.0f/255.0f, 1.0f*scale) );
//	//m_transferFunction->setPLF(plf_artifix2);

//	// default tf
//	TransferFunction::PLF plf_default;
////	plf_default.addSample( 0.0f, math::V4f(1.0f, 1.0f, 1.0f, 0.00001f*scale) );
////	plf_default.addSample( 0.266535, math::V4f(1.0f, 1.0f, 1.0f, 0.0f*scale) );
////	plf_default.addSample( 0.3f, math::V4f(1.0f, 1.0f, 1.0f, 1.0f*scale) );
////	plf_default.addSample( 1.0f, math::V4f(1.0f, 1.0f, 1.0f, 1.0f*scale) );
//	plf_default.addSample( 0.0f, math::V4f(160.0f/255.0f, 160.0f/255.0f, 164.0f/255.0f, 0.0f*scale) );
//	plf_default.addSample( 0.226535, math::V4f(68.0f/255.0f, 0.0f/255.0f, 0.0f/255.0f, 0.0f*scale) );
//	plf_default.addSample( 0.310104, math::V4f(143.0f/255.0f, 104.0f/255.0f, 54.0f/255.0f, 0.059f*scale) );
//	plf_default.addSample( 0.453057, math::V4f(18.0f/255.0f, 9.0f/255.0f, 0.0f/255.0f, 1.0f*scale) );
//	plf_default.addSample( 1.0f, math::V4f(160.0f/255.0f, 160.0f/255.0f, 164.0f/255.0f, 1.0f*scale) );

//	//	m_transferFunction->setPLF(plf_default);


	m_transferFunction2 = std::make_shared<AnimatedTransferFunction>();
//	m_transferFunction2->addPLF(0.0f, plf_1);
//	m_transferFunction2->addPLF(10.0f, plf_1);
	//m_transferFunction2->addPLF(10.0f, plf_artifix);
	//m_transferFunction2->addPLF(100.0f, plf_artifix);
	//m_transferFunction2->addPLF(5.0f, plf_1);

	// artifix tf
	m_transferFunction->addNode( 0.0f, math::V4f(160.0f/255.0f, 160.0f/255.0f, 164.0f/255.0f, 0.0f*scale) );
	m_transferFunction->addNode( 0.266535f, math::V4f(68.0f/255.0f, 0.0f/255.0f, 0.0f/255.0f, 0.0f*scale) );
	m_transferFunction->addNode( 0.310104f, math::V4f(143.0f/255.0f, 104.0f/255.0f, 54.0f/255.0f, 0.059f*scale) );
	m_transferFunction->addNode( 0.453057f, math::V4f(18.0f/255.0f, 9.0f/255.0f, 0.0f/255.0f, 1.0f*scale) );
	m_transferFunction->addNode( 1.0f, math::V4f(160.0f/255.0f, 160.0f/255.0f, 164.0f/255.0f, 1.0f*scale) );

	volumeShader->setUniform( "transferFunction", m_transferFunction->m_texture->getUniform() );
	volumeShader->setUniform( "transferFunction2", m_transferFunction2->m_texture->getUniform() );
	volumeShader->setUniform( "sigma_t_scale", 100.0f );
	volumeShader->setUniform( "shotLocalTime", 0.5f );
	//std::cout << "sigma_t_scale " << m_transferFunction->m_st_max << std::endl;





	m_debug = base::Texture2d::createRGBAFloat32(512, 512);
	std::vector<float> m_debugContent;
	m_debugContent.resize(512*512*4, 0.0f);
	m_debug->uploadRGBAFloat32( 512, 512, &m_debugContent[0] );

	m_debugFBO = base::FBO::create().width(512).height(512).attach(m_debug);






	// setup lighting --------
	math::M44f areaLightTransform = math::M44f::Identity();
	//areaLightTransform = math::M44f::TranslationMatrix(0.0f, 1.0f, 0.0f)*math::M44f::RotationMatrixX( MATH_PI*0.5 );
	//areaLightTransform = math::M44f::TranslationMatrix(0.0f, 0.0f, 1.0f)*math::M44f::RotationMatrixX( MATH_PI*0.5 );
	areaLightTransform = math::M44f::RotationMatrixX( -MATH_PIf*0.5f )*math::M44f::TranslationMatrix(0.0f, 1.3f, 0.0f);
	volumeShader->setUniform( "areaLightTransform", areaLightTransform );

	setPointLightPosition( math::V3f(1.0f, 13.4f, -13.4f) );
	setPointLightIntensity( 5000 );

	// register properties -----
	addProperty<math::V3f>( "PointLightPosition", std::bind( &Volume::getPointLightPosition, this ), std::bind( &Volume::setPointLightPosition, this, std::placeholders::_1 ) );
	addProperty<float>( "PointLightIntensity", std::bind( &Volume::getPointLightIntensity, this ), std::bind( &Volume::setPointLightIntensity, this, std::placeholders::_1 ) );

}

void Volume::setPointLightPosition( math::V3f& pos )
{
	volumeShader->setUniform( "pointLightPos", pos );
}

math::V3f Volume::getPointLightPosition()const
{
	return volumeShader->getUniform("pointLightPos")->get<math::V3f>(0);
}

void Volume::setPointLightIntensity( float intensity )
{
	volumeShader->setUniform( "pointLightIntensity", intensity );
}

float Volume::getPointLightIntensity()const
{
	return volumeShader->getUniform("pointLightIntensity")->get<float>(0);
}

void Volume::load( const std::string& filename )
{

	//base::ScalarField::Ptr density;
	m_normalizedDensity = base::ScalarField::Ptr();

	// load houdini file ================
	std::ifstream in( filename.c_str(), std::ios_base::in | std::ios_base::binary );
	houdini::HouGeo::Ptr hgeo = houdini::HouGeoIO::import( &in );
	if( hgeo )
	{
		int primIndex = 0;
		houdini::HouGeo::Primitive::Ptr prim = hgeo->getPrimitive(primIndex);

		//Volume
		if(std::dynamic_pointer_cast<houdini::HouGeo::HouVolume>(prim) )
		{
			houdini::HouGeo::HouVolume::Ptr volprim = std::dynamic_pointer_cast<houdini::HouGeo::HouVolume>(prim);
			m_normalizedDensity = volprim->field;
		}
	}

	if(!m_normalizedDensity)
		std::cerr << "unable to load " << filename << std::endl;

	//m_density->setLocalToWorld(math::M44f());

	// find density value range ---
	float densityMin=std::numeric_limits<float>::infinity();
	float densityMax=-std::numeric_limits<float>::infinity();
	for( int k = 0; k<m_normalizedDensity->m_resolution.z;++k )
		for( int j = 0; j<m_normalizedDensity->m_resolution.y;++j )
			for( int i = 0; i<m_normalizedDensity->m_resolution.x;++i )
			{
				float value = m_normalizedDensity->lvalue(i,j,k);
				densityMax = std::max(densityMax, value );
				densityMin = std::min(densityMin, value );
			}

	// normalize density ---
	for( int k = 0; k<m_normalizedDensity->m_resolution.z;++k )
		for( int j = 0; j<m_normalizedDensity->m_resolution.y;++j )
			for( int i = 0; i<m_normalizedDensity->m_resolution.x;++i )
			{
				float value = math::mapValueTo0_1(densityMin, densityMax, m_normalizedDensity->lvalue(i,j,k));
				m_normalizedDensity->lvalue(i,j,k) = value;
			}

	std::cout << "Volume::load: densityMax " << densityMax << std::endl;
	m_normalizedDensityTexture->uploadFloat32( m_normalizedDensity->m_resolution.x, m_normalizedDensity->m_resolution.y, m_normalizedDensity->m_resolution.z, m_normalizedDensity->getRawPointer() );
	volumeShader->setUniform( "normalizedDensity", m_normalizedDensityTexture->getUniform() );
	//volumeShader->setUniform( "sigma_t_max", densityMax );


	localToWorldAttr->set<math::M44f>( 0, m_normalizedDensity->m_localToWorld );
	worldToLocalAttr->set<math::M44f>( 0, m_normalizedDensity->m_worldToLocal );

	math::V3f bmin = math::transform( math::V3f(0.0f, 0.0f, 0.0f), m_normalizedDensity->m_localToWorld );
	std::cout << "bmin " << bmin.x << " " << bmin.y << " " << bmin.z << " " << std::endl;
	math::V3f bmax = math::transform( math::V3f(1.0f, 1.0f, 1.0f), m_normalizedDensity->m_localToWorld );
	std::cout << "bmax " << bmax.x << " " << bmax.y << " " << bmax.z << " " << std::endl;

	// get aabb from transformed (and therefore potentially non-axis aligned) bounding box
	math::V3f aabb_min;
	math::V3f aabb_max;
	aabb_min.x = std::min( bmin.x, bmax.x );
	aabb_min.y = std::min( bmin.y, bmax.y );
	aabb_min.z = std::min( bmin.z, bmax.z );
	aabb_max.x = std::max( bmin.x, bmax.x );
	aabb_max.y = std::max( bmin.y, bmax.y );
	aabb_max.z = std::max( bmin.z, bmax.z );
	volumeShader->setUniform( "aabb_min", aabb_min );
	volumeShader->setUniform( "aabb_max", aabb_max );
}


base::GeometryPtr Volume::createProxyGeometry()
{
	base::GeometryPtr result = base::GeometryPtr(new base::Geometry(base::Geometry::QUAD));

	// unique vertex data
	std::vector<math::Vec3f> pos;
	pos.push_back( math::Vec3f(0.0f,0.0f,1.0f) );
	pos.push_back( math::Vec3f(0.0f,1.0f,1.0f) );
	pos.push_back( math::Vec3f(1.0f,1.0f,1.0f) );
	pos.push_back( math::Vec3f(1.0f,0.0f,1.0f) );

	pos.push_back( math::Vec3f(0.0f,0.0f,0.0f) );
	pos.push_back( math::Vec3f(0.0f,1.0f,0.0f) );
	pos.push_back( math::Vec3f(1.0f,1.0f,0.0f) );
	pos.push_back( math::Vec3f(1.0f,0.0f,0.0f) );


	std::vector<math::Vec3f> uvw;
	uvw.push_back( math::Vec3f(0.0f,0.0f,1.0f) );
	uvw.push_back( math::Vec3f(0.0f,1.0f,1.0f) );
	uvw.push_back( math::Vec3f(1.0f,1.0f,1.0f) );
	uvw.push_back( math::Vec3f(1.0f,0.0f,1.0f) );

	uvw.push_back( math::Vec3f(0.0f,0.0f,0.0f) );
	uvw.push_back( math::Vec3f(0.0f,1.0f,0.0f) );
	uvw.push_back( math::Vec3f(1.0f,1.0f,0.0f) );
	uvw.push_back( math::Vec3f(1.0f,0.0f,0.0f) );

	// quads
	std::vector< std::tuple<int, int, int, int> > quads;
	quads.push_back( std::make_tuple(3, 2, 1, 0) );
	quads.push_back( std::make_tuple(4, 5, 6, 7) );
	quads.push_back( std::make_tuple(7, 6, 2, 3) );
	quads.push_back( std::make_tuple(1, 5, 4, 0) );
	quads.push_back( std::make_tuple(6, 5, 1, 2) );
	quads.push_back( std::make_tuple(4, 7, 3, 0) );

	// split per face (because we have uv shells)
	base::AttributePtr positions = base::Attribute::createV3f();
	base::AttributePtr uvwAttr = base::Attribute::createV3f();
	base::AttributePtr uvAttr = base::Attribute::createV2f();


	for( std::vector< std::tuple<int, int, int, int> >::iterator it = quads.begin(); it != quads.end(); ++it )
	{
		std::tuple<int, int, int, int> &quad = *it;
		int i0, i1, i2, i3;

		i0 = positions->appendElement( pos[std::get<0>(quad)] );
		uvwAttr->appendElement( uvw[std::get<0>(quad)] );
		uvAttr->appendElement( math::Vec2f(0.0f, 0.0f) );
		i1 = positions->appendElement( pos[std::get<1>(quad)] );
		uvwAttr->appendElement( uvw[std::get<1>(quad)] );
		uvAttr->appendElement( math::Vec2f(1.0f, 0.0f) );
		i2 = positions->appendElement( pos[std::get<2>(quad)] );
		uvwAttr->appendElement( uvw[std::get<2>(quad)] );
		uvAttr->appendElement( math::Vec2f(1.0f, 1.0f) );
		i3 = positions->appendElement( pos[std::get<3>(quad)] );
		uvwAttr->appendElement( uvw[std::get<3>(quad)] );
		uvAttr->appendElement( math::Vec2f(0.0f, 1.0f) );

		result->addQuad(i0, i1, i2, i3);			
	}

	result->setAttr( "P", positions);
	result->setAttr( "UVW", uvwAttr);
	result->setAttr( "UV", uvAttr);

	return result;
}

void Volume::render(base::Context::Ptr context, float time)
{
	m_transferFunction->updateTexture();
	float normalizedTime = time;
	if( m_transferFunction2->m_time.size()>1 )
		normalizedTime = (time - m_transferFunction2->m_time_min)/(m_transferFunction2->m_time_max-m_transferFunction2->m_time_min);
	volumeShader->setUniform( "shotLocalTime", normalizedTime );
	//std::cout << time << "->" << normalizedTime  << std::endl;
	float znear = 0.1f;
	glDisable(GL_DEPTH_TEST);
	glEnable( GL_CULL_FACE );

	// render primary pass =========================================================

	base::Context::TransformState ts;
	context->getTransformState(ts);
	context->setModelMatrix(m_normalizedDensity->m_localToWorld);

	// render back faces
	volumeBackFBO->begin();
	glFrontFace( GL_CW );
	//context->render( m_proxy, volumeGeoShader, m_density->m_localToWorld );
	context->render( m_proxy, volumeGeoShader );
	volumeBackFBO->end();

	// render front faces
	volumeFrontFBO->begin();
	glFrontFace( GL_CCW );
	//context->render( m_proxy, volumeGeoShader, m_density->m_localToWorld );
	context->render( m_proxy, volumeGeoShader );

	// computer polygon from intersection of near clipping plane with bounding box
	// calculate the near clip plane in local space of the model
	const float clipEpsilon = 0.001f;
	math::Matrix44f modelViewInverse = context->getModelViewInverseMatrix();
	math::Vec3f nearClipPlaneN;
	float nearClipPlaneDist;
	{
		math::Vec3f vsOrigin( 0, 0, 0 );
		math::Vec3f vsClipPlanePoint( 0, 0, -(znear + clipEpsilon) );
		math::Vec3f wsOrigin = math::transform(vsOrigin, modelViewInverse);
		math::Vec3f wsNearClipPlanePoint = math::transform(vsClipPlanePoint, modelViewInverse);

		nearClipPlaneN = ( wsNearClipPlanePoint - wsOrigin ).normalized();
		nearClipPlaneDist = -math::dotProduct( nearClipPlaneN, wsNearClipPlanePoint );
	}

	size_t npoints = computeAABBPlaneIntersectionGeometry(  math::Vec3f( 0.0f, 0.0f, 0.0f ), math::Vec3f( 1.0f,  1.0f,  1.0f ),
		nearClipPlaneN, nearClipPlaneDist, (math::Vec3f*)nearClipP->getRawPointer(), (math::Vec3f*)nearClipUVW->getRawPointer() );

	nearClipP->m_isDirty = true;
	nearClipUVW->m_isDirty = true;

	if( npoints )
		context->render( nearClipGeo, volumeGeoShader );


	volumeFrontFBO->end();

	context->setTransformState(ts);

	// render estimate -----------
	estimateFBO->begin();
	context->renderScreen( volumeShader );
	estimateFBO->end();

	// blur estimate -----
	blurHFBO->begin();
	context->renderScreen(blurHShader);
	blurHFBO->end();
	blurVFBO->begin();
	context->renderScreen(blurVShader);
	blurVFBO->end();

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	context->renderScreen( blurVOut );
	glDisable( GL_BLEND );



}



Volume::Ptr Volume::create()
{
	Volume::Ptr v = std::make_shared<Volume>();
	return v;
}


// reloads the shaders (for development
void Volume::reload()
{
	//volumeShader->reload();
	//dctCompute->reload();
}


REGISTERCLASS( Volume )
