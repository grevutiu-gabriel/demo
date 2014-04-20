#include "TransferFunction.h"





TransferFunction::TransferFunction()
{
	m_st_max = -std::numeric_limits<float>::max();
	m_numSamples = 2048;
	m_samples.resize(m_numSamples);
	m_texture = base::Texture1d::createRGBAFloat32(m_numSamples);


	// set default plf
	PLF plf;
	plf.addSample( -1000.0f, math::V4f(1.0f, 0.0f, 0.0f, 0.0f) );
	plf.addSample( 3095.0f, math::V4f(1.0f, 0.0f, 0.0f, 1.0f) );
	setPLF(plf);
}


void TransferFunction::setPLF( PLF plf )
{
	m_plf = plf;
	// find st_max
	for(auto it = m_plf.m_values.begin(), end = m_plf.m_values.end();it!=end;++it)
		m_st_max = std::max(m_st_max, (*it).w );
	updateTexture();
}

// bakes pfl into texture
void TransferFunction::updateTexture()
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

