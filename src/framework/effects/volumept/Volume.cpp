
#include "Volume.h"
#include <iostream>
#include <fstream>
#include <sstream>

#include <util/fs.h>
#include <util/tuple.h>
#include <gfx/Field.h>
#include <framework/houdini/HouGeoIO.h>




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

Volume::Volume()
{
}

void Volume::initialize()
{
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


	volumeBack = base::Texture2d::createRGBAFloat16( 512, 512 );
	volumeFront = base::Texture2d::createRGBAFloat16( 512, 512 );

	// setup offscreen render pass --
	volumeFrontFBO = base::FBO::create().width(512).height(512).attach(volumeFront);
	volumeBackFBO = base::FBO::create().width(512).height(512).attach(volumeBack);


	std::string basePath = "c:\\projects\\demo\\git\\src\\framework\\effects\\volumept\\";
	volumeGeoShader = base::Shader::load( basePath + "Volume.geo");

	volumeShader = base::Shader::load( basePath + "Volume" );
	volumeShader->setUniform( "volumeFront", volumeFront->getUniform() );
	volumeShader->setUniform( "volumeBack", volumeBack->getUniform() );

	// create default density field ---
	float densityMax=-std::numeric_limits<float>::infinity();
	base::ScalarField::Ptr density=base::ScalarField::create();
	for( int k = 0; k<density->m_resolution.z;++k )
		for( int j = 0; j<density->m_resolution.y;++j )
			for( int i = 0; i<density->m_resolution.x;++i )
			{
				//float t = ((float)i/(float)density->m_resolution.x);
				float t = ((float)j/(float)density->m_resolution.y);
				//float t = ((float)k/(float)density->m_resolution.z);
				//float value = t;
				//float value = 1.0f-t;
				float value = 5.0f;
				//value*=20.0f;
				density->lvalue(i,j,k) = value;
				densityMax = std::max(densityMax, value);
			}
	//std::cout << "densityMax " << densityMax << std::endl;


	m_densityTexture = base::Texture3d::createFloat32();
	m_densityTexture->uploadFloat32( density->m_resolution.x, density->m_resolution.y, density->m_resolution.z, density->getRawPointer() );
	volumeShader->setUniform( "density", m_densityTexture->getUniform() );
	// TODO: once transfer function is introduced, use max of transferfunction
	volumeShader->setUniform( "st_max", densityMax );

	math::Matrix44f localToWorld = math::M44f();
	localToWorldAttr = base::Attribute::createM44f();
	//localToWorldAttr->appendElement( density->m_localToWorld );
	localToWorldAttr->appendElement( localToWorld );
	volumeShader->setUniform( "localToWorld", localToWorldAttr );

	worldToLocalAttr = base::Attribute::createM44f();
	worldToLocalAttr->appendElement( density->m_worldToLocal );
	volumeShader->setUniform( "worldToLocal", worldToLocalAttr );


	// set defaults
	setTotalCrossSection( 20.0f );
	setAlbedo( 1.0f );
	setAbsorptionColor( math::Vec3f(0.5f,0.5f, 0.5f) );
	setScatteringColor(math::Vec3f(0.5f, 0.5f, 0.5f));

	Light l;
	l.color = math::Vec3f(1.0,1.0,1.0);
	l.exposure = 0.0f;
	setLight(0, l);



	m_debug = base::Texture2d::createRGBAFloat32(512, 512);
	std::vector<float> m_debugContent;
	m_debugContent.resize(512*512*4, 0.0f);
	m_debug->uploadRGBAFloat32( 512, 512, &m_debugContent[0] );

	m_debugFBO = base::FBO::create().width(512).height(512).attach(m_debug);




/*
	float *densityTest = m_density->download();
	i = 5;
	j = 5;
	std::cout << "density test: " << densityTest[j*10+i+0] << " " << densityTest[j*10+i+1] << " " << densityTest[j*10+i+2] << " " << densityTest[j*10+i+3] << std::endl;
	free(densityTest);
*/
}


void Volume::load( const std::string& filename )
{

	//base::ScalarField::Ptr density;
	m_density = base::ScalarField::Ptr();

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
			m_density = volprim->field;
		}
	}

	if(!m_density)
		std::cerr << "unable to load " << filename << std::endl;


	// create default density field ---
	float densityMax=-std::numeric_limits<float>::infinity();
	for( int k = 0; k<m_density->m_resolution.z;++k )
		for( int j = 0; j<m_density->m_resolution.y;++j )
			for( int i = 0; i<m_density->m_resolution.x;++i )
			{
				float value = m_density->lvalue(i,j,k);
				value = math::mapValueToRange(-1000.0f, 3095.0f, 0.0f, 1.0f, value)*10.0f;
				densityMax = std::max(densityMax, value );
			}
	std::cout << "Volume::load: densityMax " << densityMax << std::endl;
	m_densityTexture->uploadFloat32( m_density->m_resolution.x, m_density->m_resolution.y, m_density->m_resolution.z, m_density->getRawPointer() );
	volumeShader->setUniform( "density", m_densityTexture->getUniform() );
	// TODO: once transfer function is introduced, use max of transferfunction
	volumeShader->setUniform( "st_max", densityMax );
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


void Volume::render( base::Context::Ptr context, base::Camera::Ptr cam )
{
	glDisable(GL_DEPTH_TEST);
	glEnable( GL_CULL_FACE );

	// render primary pass =========================================================
	context->setView( cam->m_worldToView, cam->m_viewToWorld, cam->m_viewToNDC );

	// render back faces
	volumeBackFBO->begin();
	glFrontFace( GL_CW );
	context->render( m_proxy, volumeGeoShader, m_density->m_localToWorld );
	volumeBackFBO->end();

	// render front faces
	volumeFrontFBO->begin();
	glFrontFace( GL_CCW );
	context->render( m_proxy, volumeGeoShader, m_density->m_localToWorld );

	// computer polygon from intersection of near clipping plane with bounding box
	// calculate the near clip plane in local space of the model
	const float clipEpsilon = 0.001f;
	math::Matrix44f modelViewInverse = context->getModelViewInverseMatrix();
    math::Vec3f nearClipPlaneN;
	float nearClipPlaneDist;
    {
		math::Vec3f vsOrigin( 0, 0, 0 );
		math::Vec3f vsClipPlanePoint( 0, 0, -(cam->m_znear + clipEpsilon) );
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

	// render volume ====================

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	//m_debugFBO->begin();
	//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	context->renderScreen( volumeShader );
	//m_debugFBO->end();
	//context->renderScreen(m_debug);


	glDisable( GL_BLEND );





	/*
	std::vector<float> m_debugContent;
	m_debugContent.resize(512*512*4, 0.0f);
	glBindTexture(GL_TEXTURE_2D, m_debug->m_id);
	glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, &m_debugContent[0] );

	int i=256, j=256;
	std::cout << "ttest: " << m_debugContent[j*512+i+0] << " " << m_debugContent[j*512+i+1] << " " << m_debugContent[j*512+i+2] << " " << m_debugContent[j*512+i+3] << std::endl;
	*/

}



VolumePtr Volume::create()
{
	VolumePtr v = std::make_shared<Volume>();
	v->initialize();
	return v;
}


// reloads the shaders (for development
void Volume::reload()
{
	//volumeShader->reload();
	//dctCompute->reload();
}

// to allow external code to communicate with glsldatasource module
void Volume::setUniform( const std::string &name, base::AttributePtr uniform )
{
	//volumeShader->setUniform(name, uniform);
	//dctCompute->setUniform(name, uniform);
}

void Volume::updateCrossSectionValues()
{
	// update absorption and scattering crosssections accordingly
	float scatteringCrossSection = m_albedo*m_totalCrossSection;
	float absorptionCrossSection = (1.0f-m_albedo)*m_totalCrossSection;
	math::Vec3f finalTotalCrossSection = scatteringCrossSection*(m_scatteringColor) + absorptionCrossSection*(1.0f - m_absorptionColor);
	//volumeShader->setUniform( "totalCrossSection", finalTotalCrossSection );
	//dctCompute->setUniform( "totalCrossSection", m_totalCrossSection );
	//volumeShader->setUniform( "scatteringCrossSection", scatteringCrossSection*(m_scatteringColor) );
}

void Volume::setTotalCrossSection( float totalCrossSection )
{
	m_totalCrossSection = totalCrossSection;
	updateCrossSectionValues();
}

float Volume::getTotalCrossSection( void )
{
	return m_totalCrossSection;
}

void Volume::setAlbedo( float albedo )
{
	m_albedo = albedo;
	updateCrossSectionValues();
}
float Volume::getAlbedo( void )
{
	return m_albedo;
}

void Volume::setAbsorptionColor( math::Vec3f absorptionColor )
{
	m_absorptionColor = absorptionColor;
	updateCrossSectionValues();
}
math::Vec3f Volume::getAbsorptionColor( void )
{
	return m_absorptionColor;
}

void Volume::setScatteringColor( math::Vec3f scatteringColor )
{
	m_scatteringColor = scatteringColor;
	updateCrossSectionValues();
}
math::Vec3f Volume::getScatteringColor( void )
{
	return m_scatteringColor;
}

Volume::Light Volume::getLight( int index )
{
	return m_light0Info;
}

void Volume::setLight( int index, Light light )
{
	m_light0Info = light;
	volumeShader->setUniform( "lightColor", m_light0Info.color*pow(2.0f, m_light0Info.exposure) );
}
