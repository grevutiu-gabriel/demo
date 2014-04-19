#include "VolumeWrapper.h"









namespace gui
{


VolumeWrapper::VolumeWrapper( Volume::Ptr volume ):m_volume(volume)
{
	// compute histogram
	base::ScalarField::Ptr density = m_volume->m_normalizedDensity;
	int numBins = 256;
	std::vector<int> bins;
	bins.resize(numBins, 0);
	float min, max;
	base::field_range( *density, max, min );
	float range = max-min;
	float dx = range/float(numBins);
	std::cout << min << " " << max  <<std::endl;
	for( auto it = density->m_data.begin(), end = density->m_data.end(); it!=end;++it )
	{
		float value = *it;
		int bin = int((value-min)/dx);
		//std::cout << value/dx << std::endl;
		if( (bin >=0)&&(bin<numBins) )
			bins[bin]+=1;
	}

	m_histogram.SetBins( &bins[0], numBins );

	// propagate transfer function ---
	TransferFunction::PLF& plf = volume->m_transferFunction->m_plf;
	int numSamples = plf.m_numSamples;
	for( int i=0;i<numSamples;++i )
	{
		float x = plf.m_domain[i];
		math::V4f y = plf.m_values[i];
		m_transferFunction.AddNode(x, y.w, QColor::fromRgbF(y.x, y.y, y.z), QColor(10, 10, 10), Qt::black, 1.0f);
	}


	m_transferFunction.SetDensityScale(100.0f);
	m_transferFunction.SetShadingType(2);
	m_transferFunction.SetGradientFactor(10.0f);




	connect( &m_transferFunction, SIGNAL(Changed()), this, SLOT(transferFunctionChanged()) );
}

void VolumeWrapper::transferFunctionChanged()
{
	// update transfer function of volume
	TransferFunction::PLF plf;

	const QNodeList& nodes = m_transferFunction.GetNodes();

	for( auto it = nodes.begin(), end = nodes.end(); it!=end;++it )
	{
		const QNode& n = *it;
		plf.addSample( n.GetIntensity(), math::V4f( n.GetDiffuse().redF(), n.GetDiffuse().greenF(), n.GetDiffuse().blueF(), n.GetOpacity() ) );
	}

	m_volume->m_transferFunction->setPLF(plf);

	emit changed();
}


}//namespace gui
