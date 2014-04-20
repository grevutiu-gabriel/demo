#include "TransferFunction.h"





TransferFunction::TransferFunction() :m_isDirty(true)
{
	m_st_max = -std::numeric_limits<float>::max();
	m_numSamples = 2048;
	m_samples.resize(m_numSamples);
	m_texture = base::Texture1d::createRGBAFloat32(m_numSamples);


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





// animated transfer function --------------------

AnimatedTransferFunction::AnimatedTransferFunction()
{
	m_numSamplesDomain = 128;
	m_numSamplesTime = 128;
	int numSamples = m_numSamplesDomain*m_numSamplesTime;
	m_samples.resize(numSamples);
	m_texture = base::Texture2d::createRGBAFloat32(m_numSamplesDomain, m_numSamplesTime);
}

//AnimatedTransferFunction::AnimatedTransferFunction( const AnimatedTransferFunction& other ):
//	m_numSamplesDomain( other.m_numSamplesDomain ),
//	m_numSamplesTime( other.m_numSamplesTime ),
//	m_values(other.m_values),
//	m_domain(other.m_domain)
//{
//	int numSamples = m_numSamplesDomain*m_numSamplesTime;
//	m_samples.resize(numSamples);
//	m_texture = base::Texture2d::createRGBAFloat32(numSamples);
//}

void AnimatedTransferFunction::clear()
{
	m_values.clear();
	m_time.clear();
}


void AnimatedTransferFunction::addPLF(float time, PLF plf )
{
	m_time.push_back(time);
	m_values.push_back(plf);

	updateTexture();
}

// bakes pfl into texture
void AnimatedTransferFunction::updateTexture()
{
	m_time_min = m_time[0];
	m_time_max = m_time[m_time.size()-1];

	// update samples by sampling pfl
	for( int i=0;i<m_numSamplesDomain;++i )
	{
		float t_domain = float(i)/float(m_numSamplesDomain);

		PLF plf_currentTime;
		int vindex = 0;
		for( auto it=m_values.begin(), end=m_values.end();it!=end;++it, ++vindex )
		{
			float t_time = m_time[vindex];
			PLF& plf = *it;
			plf_currentTime.addSample( t_time, plf.evaluate(t_domain) );
		}

		for( int j=0;j<m_numSamplesTime;++j )
		{
			int index =j*m_numSamplesDomain + i;
			float t_time = plf_currentTime.m_domain[0];
			if( plf_currentTime.m_numSamples > 1 )
				t_time = (float(j)/float(m_numSamplesTime))*(m_time_max-m_time_min) + m_time_min;
			m_samples[index] = plf_currentTime.evaluate(t_time);
		}
	}


	// upload to gpu
	m_texture->uploadRGBAFloat32(m_numSamplesDomain, m_numSamplesTime, (float*)&m_samples[0]);
}

