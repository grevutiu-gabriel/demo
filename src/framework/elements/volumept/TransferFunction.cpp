#include "TransferFunction.h"





TransferFunction::TransferFunction() :m_isDirty(true)
{
	m_st_max = -std::numeric_limits<float>::max();
	m_numSamples = 2048;
	m_samples.resize(m_numSamples);
	m_texture = base::Texture1d::createRGBAFloat32(m_numSamples);


}

void TransferFunction::clear()
{
	m_nodes.clear();
	m_plf.clear();
	makeDirty();
}


//void TransferFunction::setPLF( PLF plf )
//{
//	m_plf = plf;
//	// find st_max
//	for(auto it = m_plf.m_values.begin(), end = m_plf.m_values.end();it!=end;++it)
//		m_st_max = std::max(m_st_max, (*it).w );
//	updateTexture();
//}


// bakes pfl into texture
void TransferFunction::updateTexture()
{
	if( m_isDirty )
	{
		float minx, maxx;
		m_plf.getDomainRange(minx, maxx);

		// update samples by sampling pfl
		for( int i=0;i<m_numSamples;++i )
		{
			float t = float(i)/float(m_numSamples);
			m_samples[i] = m_plf.evaluate( minx + t*(maxx-minx) );
		}

		// upload to gpu
		m_texture->uploadRGBAFloat32(m_numSamples, (float*)&m_samples[0]);

		m_isDirty = false;
	}
}

void TransferFunction::makeDirty()
{
	m_isDirty = true;
}

TransferFunctionNode::Ptr TransferFunction::addNode(float density, const math::V4f &value)
{
	TransferFunctionNode::Ptr n = std::make_shared<TransferFunctionNode>( this, int(m_nodes.size()) );
	m_nodes.push_back(n);
	m_plf.addSample( density, value );
	return n;
}

TransferFunctionNode::Ptr TransferFunction::getNode( int index )
{
	return m_nodes[index];
}

// TransferFunctionNode -------------------

TransferFunctionNode::TransferFunctionNode( TransferFunction* tf, int index ):
	m_tf(tf),
	m_index(index)
{
	m_tf->makeDirty();

	// register properties -----
	addProperty<float>( "density", std::bind( &TransferFunctionNode::getDensity, this ), std::bind( &TransferFunctionNode::setDensity, this, std::placeholders::_1 ) );
	addProperty<float>( "mappeddensity", std::bind( &TransferFunctionNode::getMappedDensity, this ), std::bind( &TransferFunctionNode::setMappedDensity, this, std::placeholders::_1 ) );
	addProperty<math::V3f>( "color", std::bind( &TransferFunctionNode::getColor, this ), std::bind( &TransferFunctionNode::setColor, this, std::placeholders::_1 ) );
}



void TransferFunctionNode::setDensity( float density )
{
	m_tf->m_plf.m_domain[m_index] = density;
	m_tf->makeDirty();
}
float TransferFunctionNode::getDensity()const
{
	return m_tf->m_plf.m_domain[m_index];
}

void TransferFunctionNode::setMappedDensity( float mappedDensity )
{
	m_tf->m_plf.m_values[m_index].w = mappedDensity;
	m_tf->makeDirty();
}
float TransferFunctionNode::getMappedDensity()const
{
	return m_tf->m_plf.m_values[m_index].w;
}
void TransferFunctionNode::setColor( const math::V3f& color)
{
	m_tf->m_plf.m_values[m_index].x = color.x;
	m_tf->m_plf.m_values[m_index].y = color.y;
	m_tf->m_plf.m_values[m_index].z = color.z;
	m_tf->makeDirty();
}

math::V3f TransferFunctionNode::getColor()const
{
	math::V4f v = m_tf->m_plf.m_values[m_index];
	return math::V3f(v.x, v.y, v.z);
}




