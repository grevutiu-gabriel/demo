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







