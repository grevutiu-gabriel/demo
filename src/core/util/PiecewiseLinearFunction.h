#pragma once

#include <vector>
#include <algorithm>





namespace base
{

	template<typename T>
	struct PiecewiseLinearFunction
	{
		PiecewiseLinearFunction()
		{
		}


		PiecewiseLinearFunction( const PiecewiseLinearFunction& other ):
			m_domain(other.m_domain),
			m_values(other.m_values),
			m_numSamples(other.m_numSamples)
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
		void getValueRange( T& miny, T& maxy )
		{
			if(m_numSamples==0)
			{
				miny = std::numeric_limits<T>::max();
				maxy = -std::numeric_limits<T>::max();
			}else
			{
				std::pair<std::vector<T>::iterator, std::vector<T>::iterator> minmax = std::minmax_element( m_values.begin(), m_values.end() );
				miny = *minmax.first;
				maxy = *minmax.second;
			}
		}
		void clear()
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

} // namespace base
