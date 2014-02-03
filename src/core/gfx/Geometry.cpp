#include "Geometry.h"
#include <iostream>

#include <math/Math.h>
#include <gltools/gl.h>
#include <util/tuple.h>
//#include "util/ObjIO.h"




namespace base
{
	Geometry::Geometry( PrimitiveType pt ) : m_primitiveType(pt), m_numPrimitives(0), m_indexBufferIsDirty(true)
	{
		// initialize indexbuffer
		glGenBuffers(1, &m_indexBufferId);

		switch( pt )
		{
		case Geometry::POINT:m_numPrimitiveVertices = 1;break;
		case Geometry::LINE:m_numPrimitiveVertices = 2;break;
		default:
		case Geometry::TRIANGLE:m_numPrimitiveVertices = 3;break;
		case Geometry::QUAD:m_numPrimitiveVertices = 4;break;
		case Geometry::POLYGON:m_numPrimitiveVertices = 0;break;
		}
	}

	AttributePtr Geometry::getAttr( const std::string &name )
	{
		std::map<std::string, AttributePtr>::iterator it = m_attributes.find(name);
		if(it != m_attributes.end())
			return it->second;
		return AttributePtr();
	}

	void Geometry::setAttr( const std::string &name, AttributePtr attr )
	{
		m_attributes[name] = attr;
	}

	bool Geometry::hasAttr( const std::string &name )
	{
		std::map<std::string, AttributePtr>::iterator it = m_attributes.find(name);
		return(it != m_attributes.end());
	}

	void Geometry::removeAttr( const std::string &name )
	{
		std::map<std::string, AttributePtr>::iterator it = m_attributes.find(name);
		if( it != m_attributes.end() )
			m_attributes.erase( it );
	}

	void Geometry::bindIndexBuffer()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferId);
		if( m_indexBufferIsDirty && !m_indexBuffer.empty() )
		{
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer.size()*sizeof(unsigned int), &m_indexBuffer[0], GL_STATIC_DRAW);
			m_indexBufferIsDirty = false;
		}
	}

	Geometry::PrimitiveType Geometry::primitiveType()
	{
		return m_primitiveType;
	}

	unsigned int Geometry::numPrimitives()
	{
		return m_numPrimitives;
	}

	// Point=1; Line=2; Triangle=3; Quad=4; polygon=*
	unsigned int Geometry::numPrimitiveVertices()
	{
		return m_numPrimitiveVertices;
	}

	unsigned int Geometry::addPoint( unsigned int vId )
	{
		m_indexBuffer.push_back(vId);
		m_indexBufferIsDirty = true;
		return m_numPrimitives++;
	}

	unsigned int Geometry::addLine( unsigned int vId0, unsigned int vId1 )
	{
		m_indexBuffer.push_back(vId0);
		m_indexBuffer.push_back(vId1);
		m_indexBufferIsDirty = true;
		return m_numPrimitives++;
	}

	unsigned int Geometry::addTriangle( unsigned int vId0, unsigned int vId1, unsigned int vId2 )
	{
		m_indexBuffer.push_back(vId0);
		m_indexBuffer.push_back(vId1);
		m_indexBuffer.push_back(vId2);
		m_indexBufferIsDirty = true;
		return m_numPrimitives++;
	}

	unsigned int Geometry::addQuad( unsigned int vId0, unsigned int vId1, unsigned int vId2, unsigned int vId3 )
	{
		m_indexBuffer.push_back(vId0);
		m_indexBuffer.push_back(vId1);
		m_indexBuffer.push_back(vId2);
		m_indexBuffer.push_back(vId3);
		m_indexBufferIsDirty = true;
		return m_numPrimitives++;
	}

	unsigned int Geometry::addPolygonVertex( unsigned int v )
	{
		m_indexBuffer.push_back(v);
		++m_numPrimitiveVertices;
		m_indexBufferIsDirty = true;
		if( !m_numPrimitives )
			// only 0 or 1 number of primitives allowed with polygons
			m_numPrimitives = 1;
		return m_numPrimitives;
	}



	// reverses the order of vertices for each primitive (CW polys become CCW)
	void Geometry::reverse()
	{
		for( unsigned int i=0;i<m_numPrimitives;++i )
		{
			int offset = i*m_numPrimitiveVertices;
			std::reverse(m_indexBuffer.begin()+offset, m_indexBuffer.begin()+offset+m_numPrimitiveVertices );
		}
		m_indexBufferIsDirty = true;
	}

	// duplicates point and returns index of duplicate (all attributes are copied etc.)
	unsigned int Geometry::duplicatePoint( unsigned int index )
	{
		int newIndex = getAttr("P")->numElements();
		for( std::map< std::string, AttributePtr >::iterator it = m_attributes.begin(); it != m_attributes.end(); ++it )
		{
			AttributePtr attr = it->second;
			attr->resize( newIndex+1 );
			memcpy( attr->getRawPointer(newIndex), attr->getRawPointer(index), attr->numComponents()*attr->elementComponentSize() );
		}
		return newIndex;
	}


	//
	// removes all attributes and primitives
	//
	void Geometry::clear()
	{
		for( std::map< std::string, AttributePtr >::iterator it = m_attributes.begin(); it != m_attributes.end(); ++it )
			it->second->clear();

		m_indexBuffer.clear();
		m_numPrimitives = 0;
		m_indexBufferIsDirty = true;
	}

	Geometry::Ptr Geometry::createPointGeometry()
	{
		GeometryPtr result = GeometryPtr( new Geometry(POINT) );
		AttributePtr positions = AttributePtr( Attribute::createV3f() );
		result->setAttr( "P", positions);
		return result;
	}

	Geometry::Ptr Geometry::createLineGeometry()
	{
		GeometryPtr result = GeometryPtr( new Geometry(LINE) );
		AttributePtr positions = AttributePtr( Attribute::createV3f() );
		result->setAttr( "P", positions);
		return result;
	}


	Geometry::Ptr Geometry::createTriangleGeometry()
	{
		GeometryPtr result = GeometryPtr( new Geometry(TRIANGLE) );
		AttributePtr positions = AttributePtr( Attribute::createV3f() );
		result->setAttr( "P", positions);
		return result;
	}

	Geometry::Ptr Geometry::createQuadGeometry()
	{
		Geometry::Ptr result = Geometry::Ptr( new Geometry(QUAD) );
		Attribute::Ptr positions = AttributePtr( Attribute::createV3f() );
		result->setAttr( "P", positions);
		return result;
	}

	Geometry::Ptr Geometry::createPolyGeometry()
	{
		Geometry::Ptr result = std::make_shared<Geometry>(POLYGON);
		AttributePtr positions = AttributePtr( Attribute::createV3f() );
		result->setAttr( "P", positions);
		return result;
	}
/*
	GeometryPtr Geometry::createReferenceMesh()
	{
		return importObj( path( "base" ) + "/data/meshref.obj" );
	}
*/
/*
	GeometryPtr geo_points( math::Vec3f p )
	{
		GeometryPtr result = GeometryPtr( new Geometry(Geometry::POINT) );
		AttributePtr positions = Attribute::createVec3f();
		positions->appendElement( p );
		result->setAttr( "P", positions);
		result->addPoint( 0 );
		return result;
	}

*/

	Geometry::Ptr Geometry::createQuad(Geometry::PrimitiveType primType)
	{
		Geometry::Ptr result = std::make_shared<Geometry>(primType);

		AttributePtr positions = Attribute::createV3f();
		AttributePtr uvs = Attribute::createV2f();

		positions->appendElement( math::Vec3f(-1.0f,-1.0f,0.0f) );
		uvs->appendElement( .0f, .0f );
		positions->appendElement( math::Vec3f(-1.0f,1.0f,0.0f) );
		uvs->appendElement( .0f, 1.0f );
		positions->appendElement( math::Vec3f(1.0f,1.0f,0.0f) );
		uvs->appendElement( 1.0f, 1.0f );
		positions->appendElement( math::Vec3f(1.0f,-1.0f,0.0f) );
		uvs->appendElement( 1.0f, .0f );

		result->setAttr( "P", positions);
		result->setAttr( "UV", uvs );

		if( primType == Geometry::QUAD )
			result->addQuad( 3, 2, 1, 0 );
		else if( primType == Geometry::TRIANGLE )
		{
			result->addTriangle( 3, 2, 1 );
			result->addTriangle( 3, 1, 0 );
		}

		return result;
	}

	Geometry::Ptr Geometry::createGrid( int xres, int zres, Geometry::PrimitiveType primType )
	{
		Geometry::Ptr result = std::make_shared<Geometry>(primType);

		AttributePtr positions = Attribute::createV3f();
		result->setAttr( "P", positions);

		AttributePtr uvs = Attribute::createV2f();
		result->setAttr( "UV", uvs );

		for( int j=0; j<zres; ++j )
			for( int i=0; i<xres; ++i )
			{
				float u = i/(float)(xres-1);
				float v = j/(float)(zres-1);
				positions->appendElement( math::V3f(u-0.5f,0.0f,v-0.5f) );
				uvs->appendElement( u, 1.0f - v ); // 1.0 - v because opengl texture space
			}

		if( primType == Geometry::POINT )
		{
			int numPoints = xres*zres;
			for( int i=0;i<numPoints;++i )
				result->addPoint( i );
		}else
		if( primType == Geometry::LINE )
		{
			for( int j=0; j<zres; ++j )
			{
				for( int i=0; i<xres-1; ++i )
				{
					int vo = (j*xres);
					result->addLine( vo+i, vo+i+1 );
				}
			}
			for( int j=0; j<xres; ++j )
			{
				for( int i=0; i<zres-1; ++i )
				{
					int v0 = (i*xres) + j;
					int v1 = ((i+1)*xres) + j;
					result->addLine( v0, v1 );
				}
			}
		}else
		if( primType == Geometry::TRIANGLE )
			for( int j=0; j<zres-1; ++j )
				for( int i=0; i<xres-1; ++i )
				{
					int vo = (j*xres);
					result->addTriangle( vo+xres+i+1, vo+i+1, vo+i );
					result->addTriangle( vo+xres+i, vo+xres+i+1, vo+i );

				}


		return result;
	}
/*
	GeometryPtr geo_cube( const math::BoundingBox3f &bound, Geometry::PrimitiveType primType )
	{
		GeometryPtr result = GeometryPtr(new Geometry(primType));

		// unique points
		std::vector<math::Vec3f> pos;
		pos.push_back( math::Vec3f(bound.minPoint.x,bound.minPoint.y,bound.maxPoint.z) );
		pos.push_back( math::Vec3f(bound.minPoint.x,bound.maxPoint.y,bound.maxPoint.z) );
		pos.push_back( math::Vec3f(bound.maxPoint.x,bound.maxPoint.y,bound.maxPoint.z) );
		pos.push_back( math::Vec3f(bound.maxPoint.x,bound.minPoint.y,bound.maxPoint.z) );

		pos.push_back( math::Vec3f(bound.minPoint.x,bound.minPoint.y,bound.minPoint.z) );
		pos.push_back( math::Vec3f(bound.minPoint.x,bound.maxPoint.y,bound.minPoint.z) );
		pos.push_back( math::Vec3f(bound.maxPoint.x,bound.maxPoint.y,bound.minPoint.z) );
		pos.push_back( math::Vec3f(bound.maxPoint.x,bound.minPoint.y,bound.minPoint.z) );

		if( primType == Geometry::QUAD )
		{


			// quads
			std::vector< std::tuple<int, int, int, int> > quads;
			quads.push_back( std::make_tuple(3, 2, 1, 0) );
			quads.push_back( std::make_tuple(4, 5, 6, 7) );
			quads.push_back( std::make_tuple(7, 6, 2, 3) );
			quads.push_back( std::make_tuple(1, 5, 4, 0) );
			quads.push_back( std::make_tuple(6, 5, 1, 2) );
			quads.push_back( std::make_tuple(4, 7, 3, 0) );

			// split per face (because we have uv shells)
			AttributePtr positions = Attribute::createVec3f();
			AttributePtr uv = Attribute::createVec2f();


			for( std::vector< std::tuple<int, int, int, int> >::iterator it = quads.begin(); it != quads.end(); ++it )
			{
				std::tuple<int, int, int, int> &quad = *it;
				int i0, i1, i2, i3;

				i0 = positions->appendElement( pos[std::get<0>(quad)] );
				uv->appendElement( math::Vec2f(0.0f, 0.0f) );
				i1 = positions->appendElement( pos[std::get<1>(quad)] );
				uv->appendElement( math::Vec2f(1.0f, 0.0f) );
				i2 = positions->appendElement( pos[std::get<2>(quad)] );
				uv->appendElement( math::Vec2f(1.0f, 1.0f) );
				i3 = positions->appendElement( pos[std::get<3>(quad)] );
				uv->appendElement( math::Vec2f(0.0f, 1.0f) );

				result->addQuad(i0, i1, i2, i3);			
			}

			result->setAttr( "P", positions);
			result->setAttr( "UV", uv);
		}else
		if( primType == Geometry::LINE )
		{
			// points
			AttributePtr positions = Attribute::createVec3f();
			for( std::vector<math::Vec3f>::iterator it = pos.begin(), end=pos.end(); it != end; ++it)
				positions->appendElement( *it );
			result->setAttr( "P", positions);
			//lines
			result->addLine( 0, 1 );
			result->addLine( 1, 2 );
			result->addLine( 2, 3 );
			result->addLine( 3, 0 );

			result->addLine( 4, 5 );
			result->addLine( 5, 6 );
			result->addLine( 6, 7 );
			result->addLine( 7, 4 );

			result->addLine( 0, 4 );
			result->addLine( 1, 5 );
			result->addLine( 2, 6 );
			result->addLine( 3, 7 );
		}

		return result;
	}

	//TODO: add uv mapping
	GeometryPtr geo_sphere( int uSubdivisions, int vSubdivisions, float radius, math::Vec3f center, Geometry::PrimitiveType primType )
	{
		GeometryPtr result = GeometryPtr(new Geometry(primType));

		AttributePtr positions = Attribute::createVec3f();
		result->setAttr( "P", positions);

		AttributePtr uvs = Attribute::createVec2f();
		result->setAttr( "UV", uvs );

		float dPhi = MATH_2PIf/uSubdivisions;
		float dTheta = MATH_PIf/vSubdivisions;
		float theta, phi;

		// y
		for (theta=MATH_PIf/2.0f+dTheta;theta<=(3.0f*MATH_PIf)/2.0f-dTheta;theta+=dTheta)
		{
			math::Vec3f p;
			float y = sin(theta);
			// x-z
			phi = 0.0f;
			for( int j = 0; j<uSubdivisions; ++j  )
			{
				p.x = cos(theta) * cos(phi);
				p.y = y;
				p.z = cos(theta) * sin(phi);

				p = p*radius + center;

				positions->appendElement( p );
				phi+=dPhi;
			}
		}

		int pole1 = positions->appendElement( math::Vec3f(0.0f, 1.0f, 0.0f)*radius + center );
		int pole2 = positions->appendElement( math::Vec3f(0.0f, -1.0f, 0.0f)*radius + center );

		if( primType == Geometry::POINT )
		{
			int numVertices = positions->numElements();
			for( int i=0; i< numVertices; ++i )
				result->addPoint( i );
		}else
		if( primType == Geometry::LINE )
		{
			int numVertices = positions->numElements();
			for( int j=0; j<vSubdivisions-3;++j )
			{
				int offset = j*(uSubdivisions);
				int i = 0;
				for( i=0; i<uSubdivisions-1; ++i )
					result->addLine(offset+i, offset+i+1);
				result->addLine(offset+0, offset+i);
			}
		}else
		if( primType == Geometry::TRIANGLE )
		{
			// add faces
			for( int j=0; j<vSubdivisions-3;++j )
			{
				int offset = j*(uSubdivisions);
				int i = 0;
				for( i=0; i<uSubdivisions-1; ++i )
				{
					result->addTriangle(offset+i+1, offset+i + uSubdivisions, offset+i);
					result->addTriangle(offset+i+1, offset+i+uSubdivisions+1, offset+i + uSubdivisions);
				}
				result->addTriangle(offset+0,offset+i + uSubdivisions,offset+i);
				result->addTriangle(offset,offset + uSubdivisions,offset+i + uSubdivisions);
			}
			for( int i=0; i<uSubdivisions-1; ++i )
			{
				result->addTriangle(i+1, i,pole1);
				result->addTriangle(uSubdivisions*(vSubdivisions-3)+i, uSubdivisions*(vSubdivisions-3)+i+1, pole2);
			}
			result->addTriangle(0, uSubdivisions-1, pole1);
			result->addTriangle(uSubdivisions*(vSubdivisions-2)-1, uSubdivisions*(vSubdivisions-3), pole2);
		}
		return result;
	}

	GeometryPtr geo_circle( int uSubdivisions, float radius, math::Vec3f center, Geometry::PrimitiveType primType )
	{
		GeometryPtr result = GeometryPtr(new Geometry(Geometry::LINE));

		AttributePtr positions = Attribute::createVec3f();
		result->setAttr( "P", positions);

		AttributePtr uvs = Attribute::createVec2f();
		result->setAttr( "UV", uvs );

		float dPhi = MATH_2PIf/uSubdivisions;
		float phi;

		{
			math::Vec3f p;
			// x-z
			phi = 0.0f;
			for( int j = 0; j<uSubdivisions; ++j  )
			{
				p.x = cos(phi);
				p.y = 0.0f;
				p.z = sin(phi);

				p = p*radius + center;

				positions->appendElement( p );
				phi+=dPhi;
			}
		}

		int pole1 = positions->appendElement( math::Vec3f(0.0f, 1.0f, 0.0f)*radius + center );
		int pole2 = positions->appendElement( math::Vec3f(0.0f, -1.0f, 0.0f)*radius + center );

		if( primType == Geometry::POINT )
		{
			int numVertices = positions->numElements();
			for( int i=0; i< numVertices; ++i )
				result->addPoint( i );
		}else
		if( primType == Geometry::LINE )
		{
			for( int i=0; i<uSubdivisions-1; ++i )
			{
				result->addLine(i, i+1);
			}
			result->addLine(uSubdivisions-1, 0);

//			// add faces
//			for( int j=0; j<vSubdivisions-3;++j )
//			{
//				int offset = j*(uSubdivisions);
//				int i = 0;
//				for( i=0; i<uSubdivisions-1; ++i )
//				{
//					result->addTriangle(offset+i+1, offset+i + uSubdivisions, offset+i);
//					result->addTriangle(offset+i+1, offset+i+uSubdivisions+1, offset+i + uSubdivisions);
//				}
//				result->addTriangle(offset+0,offset+i + uSubdivisions,offset+i);
//				result->addTriangle(offset,offset + uSubdivisions,offset+i + uSubdivisions);
//			}
//			for( int i=0; i<uSubdivisions-1; ++i )
//			{
//				result->addTriangle(i+1, i,pole1);
//				result->addTriangle(uSubdivisions*(vSubdivisions-3)+i, uSubdivisions*(vSubdivisions-3)+i+1, pole2);
//			}
//			result->addTriangle(0, uSubdivisions-1, pole1);
//			result->addTriangle(uSubdivisions*(vSubdivisions-2)-1, uSubdivisions*(vSubdivisions-3), pole2);

		}
		return result;
	}

	GeometryPtr geo_cone( math::Vec3f axis, float halfAngle,  float height, int uSubdivisions )
	{
		GeometryPtr result = GeometryPtr(new Geometry(Geometry::TRIANGLE));

		AttributePtr positions = Attribute::createVec3f();
		result->setAttr( "P", positions);


		float r = height*tan(halfAngle);

		math::Matrix33f rm = math::Matrix33f::RotationMatrix( axis, (2.0f*MATH_PIf)/uSubdivisions );

		math::Vec3f p = height*axis+math::cross( math::baseVec3<float>( math::nondominantAxis(axis) ), axis )*r;

		// add apex
		positions->appendElement<math::Vec3f>(math::Vec3f(0.0f, 0.0f, 0.0f));

		// add base and triangles
		positions->appendElement<math::Vec3f>(p);
		float d = (p-height*axis).getLength();
		for( int i=1;i<uSubdivisions;++i )
		{
			p = math::transform( p, rm );

			d = (p-height*axis).getLength();

			positions->appendElement<math::Vec3f>(p);
			result->addTriangle( 0, i, i+1 );
		}


		return result;
	}










	void apply_transform( GeometryPtr geo, math::Matrix44f tm )
	{
		AttributePtr pAttr = geo->getAttr("P");
		int numElements = pAttr->numElements();
		for( int i=0; i<numElements; ++i )
		{
			math::Vec3f v = math::transform( pAttr->get<math::Vec3f>(i), tm);
			pAttr->set<math::Vec3f>( (unsigned int)i, v );
		}
	}



	//
	// computes vertex normals
	// Assumes geometry to be triangles!
	//
	void apply_normals( GeometryPtr geo )
	{
		// only works with triangles and quads
		if( !((geo->primitiveType() == Geometry::TRIANGLE)||(geo->primitiveType() == Geometry::QUAD)) )
		{
			std::cerr << "apply_normals: can compute normals only on non triangle or quad geometry\n";
			return;
		}

		AttributePtr normalAttr = geo->getAttr("N");
		
		if( !normalAttr )
			normalAttr = Attribute::createVec3f();
		else
			normalAttr->clear();

		AttributePtr positions = geo->getAttr("P");
		int numPoints = positions->numElements();
		for( int i=0; i < numPoints; ++i )
			normalAttr->appendElement( math::Vec3f(0.0f, 0.0f, 0.0f) );

		int numPrimitives = geo->numPrimitives();
		int numPrimitiveVertices = geo->numPrimitiveVertices();
		for( int i=0; i < numPrimitives; ++i )
		{
			int idx[3];
			idx[0] = geo->m_indexBuffer[i*numPrimitiveVertices];
			idx[1] = geo->m_indexBuffer[i*numPrimitiveVertices+1];
			idx[2] = geo->m_indexBuffer[i*numPrimitiveVertices+2];

			math::Vec3f v1 = positions->get<math::Vec3f>( idx[1] )-positions->get<math::Vec3f>( idx[0] );
			math::Vec3f v2 = positions->get<math::Vec3f>( idx[2] )-positions->get<math::Vec3f>( idx[0] );
			math::Vec3f fn = math::normalize( math::crossProduct( v1,v2 ) );

			for( int j=0; j<3; ++j )
				normalAttr->set<math::Vec3f>( idx[j], normalAttr->get<math::Vec3f>(idx[j])+fn );
		}

		for( int i=0; i < numPoints; ++i )
			normalAttr->set<math::Vec3f>( i, math::normalize(  normalAttr->get<math::Vec3f>(i) ) );

		geo->setAttr( "N", normalAttr );
	}





	math::BoundingBox3f compute_bound( GeometryPtr geo )
	{
		math::BoundingBox3f bbox;
		AttributePtr p = geo->getAttr( "P" );
		int numElements = p->numElements();
		for( int i=0;i<numElements;++i )
			bbox.extend(p->get<math::Vec3f>(i));
		return bbox;
	}
	*/
}





