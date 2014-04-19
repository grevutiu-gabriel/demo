#pragma once

#include <QObject>

#include "../framework/elements/volumept/Volume.h"
#include "../../widgets/TFEditor/Histogram.h"
#include "../../widgets/TFEditor/TransferFunction.h"


namespace gui
{

class VolumeWrapper : public QObject
{
	Q_OBJECT
public:
	typedef std::shared_ptr<VolumeWrapper> Ptr;

	VolumeWrapper( Volume::Ptr volume );

	QHistogram& getHistogram()
	{
		return m_histogram;
	}
	QTransferFunction& getTransferFunction()
	{
		return m_transferFunction;
	}
signals:
	void changed();
public slots:
	void transferFunctionChanged();
public:
	Volume::Ptr m_volume;
	QHistogram          m_histogram;
	QTransferFunction   m_transferFunction;
};

} // namespace gui
