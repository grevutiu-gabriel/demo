#pragma once

#include <util/shared_ptr.h>
#include <gltools/gl.h>
#include <gltools/misc.h>
#include <util/StringManip.h>
#include <util/Path.h>
#include <gfx/Geometry.h>
#include <gfx/Shader.h>
#include <gfx/Texture.h>
#include <gfx/Image.h>
#include <gfx/Context.h>
#include <gfx/FBO.h>
#include <gfx/Field.h>


template<typename T>
struct PiecewiseLinearFunction
{
	PiecewiseLinearFunction()
	{
	}

	~PiecewiseLinearFunction()
	{
	}

	void getDomainRange( float& minx, float& maxx )
	{
		if(m_numSamples==0)
		{
			minx = std::numeric_limits<float>::max();
			maxx = -std::numeric_limits<float>::max();
		}else
		{
			minx = m_domain[0];
			maxx = m_domain[m_numSamples-1];
		}
	}


	/*
	HOST void set( float *domain_host, T *values_host, int numSamples )
	{
		m_numSamples = numSamples;

		CudaSafeCall( cudaDeviceSynchronize() );
		CudaSafeCall( cudaFree( this->m_domain ) );
		CudaSafeCall( cudaFree( this->m_values ) );

		// upload data to gpu mem ---
		CudaSafeCall( cudaMalloc( &this->m_domain, m_numSamples*sizeof(float) ) );
		CudaSafeCall( cudaMalloc( &this->m_values, m_numSamples*sizeof(T) ) );

		CudaSafeCall( cudaMemcpy( this->m_domain, domain_host, m_numSamples*sizeof(float), cudaMemcpyHostToDevice ));
		CudaSafeCall( cudaMemcpy( this->m_values, values_host, m_numSamples*sizeof(T), cudaMemcpyHostToDevice ));
	}
	*/
	void reset()
	{
		m_domain.clear();
		m_values.clear();
		m_numSamples = 0;
	}

	void addSample( float x, T y )
	{
		m_domain.push_back(x);
		m_values.push_back(y);
		m_numSamples = (int)m_domain.size();
	}

	T evaluate( float x )
	{
		if( m_numSamples == 0 )
			return T(0.0f);

		// out of bound cases
		if( x <= m_domain[0] )
			return m_values[0];
		if( x >= m_domain[m_numSamples-1] )
			return m_values[m_numSamples-1];

		// find interval using binary search http://en.wikipedia.org/wiki/Binary_search_algorithm#Deferred_detection_of_equality
		int imin = 0;
		int imax = m_numSamples - 1;

		while( imin < imax )
		{
			int imid = (imax + imin)/2 + 1;
			if( m_domain[imid] > x )
				imax = imid - 1;
			else
				imin = imid;
		};

		return math::lerp( m_values[imax], m_values[imax+1], (x-m_domain[imax])/(m_domain[imax+1]-m_domain[imax]) );
	}

	std::vector<float>                  m_domain;
	std::vector<T>                      m_values;
	int                                 m_numSamples;
};


struct TransferFunction
{
	typedef std::shared_ptr<TransferFunction> Ptr;
	typedef PiecewiseLinearFunction<math::V4f> PLF;

	TransferFunction();

	void                                   setPLF( PLF plf );
	void                                   updateTexture(); // bakes pfl into texture


	PiecewiseLinearFunction<math::V4f>     m_plf;
	int                                    m_numSamples;
	std::vector<math::V4f>                 m_samples;
	base::Texture1d::Ptr                   m_texture;
	float                                  m_st_max;
};

