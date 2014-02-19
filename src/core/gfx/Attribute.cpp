#include "Attribute.h"

#include <iostream>

#include <gltools/gl.h>
#include <gltools/misc.h>
#include <util/types.h>


namespace base
{
	int Attribute::g_nextTextureUnit = 0; // used for assigning texture units to sampler uniform attributes (reset in Context::bind)

	Attribute::Attribute( char numComponents, ComponentType componentType, int textureTarget ) : m_numElements(0), m_numComponents(numComponents), m_isDirty(true), m_textureTarget(textureTarget), m_componentType(componentType)
	{
		switch(componentType)
		{
		case INT:
			{
				m_internalComponentType = GL_INT;
				m_componentSize=sizeof(int);
			}break;
		default:
		case FLOAT:
			{
				m_internalComponentType = GL_FLOAT;
				m_componentSize=sizeof(float);
			}break;
		case SAMPLER:
			{
				m_internalComponentType = SAMPLER;
				m_componentSize=sizeof(int);
			}break;
		};

		glGenBuffers(1, &m_bufferId);
	}

	Attribute::~Attribute()
	{
		glDeleteBuffers(1, &m_bufferId);
	}

	AttributePtr Attribute::copy()
	{
		return create( numComponents(), elementComponentType(), (unsigned char *)getRawPointer(), numElements() );
	}

	Attribute::ComponentType Attribute::elementComponentType()
	{
		return m_componentType;
	}

	void Attribute::bindAsAttribute( int index )
	{
		if(m_isDirty && m_numElements)
		{
			// activate and specify pointer to vertex array
			// should be done only when attribute has been updated
			glBindBuffer(GL_ARRAY_BUFFER, m_bufferId);
			glBufferData(GL_ARRAY_BUFFER, numComponents()*elementComponentSize()*numElements(), getRawPointer(), GL_STATIC_DRAW);
			m_isDirty = false;
		}

		glBindBuffer(GL_ARRAY_BUFFER, m_bufferId);
		glEnableVertexAttribArray(index);
		if( m_internalComponentType == GL_FLOAT )
			glVertexAttribPointer(index, numComponents(), m_internalComponentType, false, 0, 0);
		else
			glVertexAttribIPointer(index, numComponents(), m_internalComponentType, 0, 0);

	}
	void Attribute::unbindAsAttribute( int index )
	{
		// deactivate vertex arrays after drawing
		glDisableVertexAttribArray(index);
	}

	void Attribute::bindAsUniform( int index )
	{
		switch( numComponents() )
		{
		case 1:
			if( m_internalComponentType == GL_FLOAT )
			{
				//printf("setting uniform tmp: %f at uniform location %i\n", *((float *)getRawPointer()), index );
				glUniform1fv( index, numElements(), (float *)getRawPointer());
			}
			else if( m_internalComponentType == GL_INT )
			{
				glUniform1iv( index, numElements(), (int*)getRawPointer());
			}else if( m_internalComponentType == SAMPLER )
			{
				int *texIds = (int*)getRawPointer();
				int textureUnits[32];
				int num = numElements();
				for( int i=0;i<num;++i )
				{
					glActiveTexture(GL_TEXTURE0+g_nextTextureUnit);
					glBindTexture(m_textureTarget, *texIds);
					textureUnits[i] = g_nextTextureUnit;
					++texIds;
					++g_nextTextureUnit;
				}
				glUniform1iv( index, num, textureUnits);
			}
			break;
		case 2:
			if( m_internalComponentType == GL_FLOAT )
				glUniform2fv( index, numElements(), (float *)getRawPointer());
			break;
		case 3:
			if( m_internalComponentType == GL_FLOAT )
				glUniform3fv( index, numElements(), (float *)getRawPointer());
			break;
		case 4:
			if( m_internalComponentType == GL_FLOAT )
				glUniform4fv( index, numElements(), (float *)getRawPointer());
			break;
		case 9:
			glUniformMatrix3fv( index, numElements(), false, (float *)getRawPointer() );
			break;
		case 16:
			glUniformMatrix4fv( index, numElements(), false, (float *)getRawPointer() );
			break;
		};
	}
	void Attribute::unbindAsUniform( int index )
	{
		// ?
		// for better texture unit management tell the currently used texture unit is not used anymore
	}


	AttributePtr Attribute::create(char numComponents, ComponentType componentType, unsigned char *raw, int numElements)
	{
		AttributePtr attr = std::make_shared<Attribute>( numComponents, componentType );

		attr->m_numElements = numElements;
		int size = attr->elementComponentSize()*attr->numComponents()*attr->numElements();
		attr->m_data.resize( size );
		memcpy( &attr->m_data[0], raw, size );

		return attr;
	}

	
	AttributePtr Attribute::createSamplerCube()
	{
		return AttributePtr( new Attribute(1, Attribute::SAMPLER, GL_TEXTURE_CUBE_MAP) );
	}

	AttributePtr Attribute::createSampler3d()
	{
		return AttributePtr( new Attribute(1, Attribute::SAMPLER, GL_TEXTURE_3D) );
	}

	AttributePtr Attribute::createSampler2d()
	{
		return AttributePtr( new Attribute(1, Attribute::SAMPLER, GL_TEXTURE_2D) );
	}

	AttributePtr Attribute::createSampler2dMS()
	{
		return AttributePtr( new Attribute(1, Attribute::SAMPLER, GL_TEXTURE_2D_MULTISAMPLE) );
	}

	AttributePtr Attribute::createSampler2dArray()
	{
		return AttributePtr( new Attribute(1, Attribute::SAMPLER, GL_TEXTURE_2D_ARRAY) );
	}

	AttributePtr Attribute::createSampler1d()
	{
		return AttributePtr( new Attribute(1, Attribute::SAMPLER, GL_TEXTURE_1D) );
	}

	AttributePtr Attribute::createM33f()
	{
		return AttributePtr( new Attribute(9, Attribute::FLOAT) );
	}

	AttributePtr Attribute::createM44f()
	{
		return AttributePtr( new Attribute(16, Attribute::FLOAT) );
	}

	AttributePtr Attribute::createV4f( int numElements )
	{
		AttributePtr attr = AttributePtr( new Attribute(4, Attribute::FLOAT) );
		attr->resize(numElements);
		return attr;
	}

	AttributePtr Attribute::createV3f( int numElements )
	{
		AttributePtr attr = AttributePtr( new Attribute(3, Attribute::FLOAT) );
		attr->resize(numElements);
		return attr;
	}

	AttributePtr Attribute::createV2f( int numElements )
	{
		AttributePtr attr = AttributePtr( new Attribute(2, Attribute::FLOAT) );
		attr->resize(numElements);
		return attr;
	}

	AttributePtr Attribute::createFloat( int numElements )
	{
		AttributePtr attr = AttributePtr( new Attribute(1, Attribute::FLOAT) );
		attr->resize(numElements);
		return attr;
	}

	AttributePtr Attribute::createInt( int numElements )
	{
		AttributePtr attr = AttributePtr( new Attribute(1, Attribute::INT) );
		attr->resize(numElements);
		return attr;
	}

/*
Attribute *Attribute::copy()
{
	Attribute *nattr = new Attribute( numComponents(), elementComponentSize() );
	nattr->m_data.resize( numElements() * numComponents()*elementComponentSize() );
	msys_memcpy( nattr->m_data.m_data, m_data.m_data, numElements() * numComponents()*elementComponentSize() );
	nattr->m_numElements = numElements();
	return nattr;
}




math::Vec3f Attribute::getVec3f( int vertexIndex )
{
	float *data = (float*)&m_data.m_data[vertexIndex * sizeof(math::Vec3f)];
	float f0 = *data;data++;
	float f1 = *data;data++;
	float f2 = *data;
	return math::Vec3f( f0, f1, f2 );
}

void Attribute::getElement( int vertexIndex, void *mem )
{
	if(vertexIndex<numElements())
		msys_memcpy( mem, &m_data.m_data[vertexIndex * numComponents()*elementComponentSize()], numComponents()*elementComponentSize() );
}

int Attribute::appendElement( void *mem )
{
	int pos = m_data.size();
	m_data.resize( pos + numComponents()*elementComponentSize() );
	msys_memcpy( &m_data.m_data[pos], mem, numComponents()*elementComponentSize() );
	return m_numElements++;
}

int Attribute::appendElements( int num )
{
	m_data.resize( m_data.size() + num*numComponents()*elementComponentSize() );
	m_numElements += num;
	return m_numElements;
}

void Attribute::setElement( int vertexIndex, void *mem )
{
	if(vertexIndex<numElements())
		msys_memcpy( &m_data.m_data[vertexIndex * numComponents()*elementComponentSize()], mem, numComponents()*elementComponentSize() );
}


void Attribute::removeElement( int vertexIndex )
{
	m_data.erase( vertexIndex*numComponents()*elementComponentSize(), numComponents()*elementComponentSize() );
	--m_numElements;
}

*/

	int Attribute::componentSize( ComponentType ct )
	{
		switch(ct)
		{
		case INT:return sizeof(sint32);
		case FLOAT:return sizeof(real32);
		//case REAL64:return sizeof(real64);
		default:
			throw std::runtime_error( "unknown component type" );
		};
		return -1;
	}


	Attribute::ComponentType Attribute::componentType( const std::string& ct )
	{
		if( ct == "fpreal32" )
			return FLOAT;
		else
		if( ct == "float" )
			return FLOAT;
		else
		//if( ct == "fpreal64" )
		//	return REAL64;
		//else
		if( ct == "int32" )
			return INT;
		else
		if( ct == "int" )
			return INT;
		return INVALID;
	}


}
