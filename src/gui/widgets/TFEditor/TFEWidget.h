#pragma once

#include <QGroupBox>
#include <QGridLayout>
#include "TFView.h"
//#include "GradientRamp.h"

#include "../../wrapper/elements/VolumeWrapper.h"

namespace gui
{

class TFEWidget : public QGroupBox
{
	Q_OBJECT

public:
	TFEWidget(VolumeWrapper::Ptr volume, QWidget* pParent = NULL);

public slots:
	//void OnRenderBegin(void);
	//void OnRenderEnd(void);
	//void OnUpdateGradients(void);

protected:
	QGridLayout			m_MainLayout;
	QGridLayout			m_TopLeftLayout;
	QGridLayout			m_TopLayout;
	QGridLayout			m_TopRightLayout;
	QGridLayout			m_LeftLayout;
	QGridLayout			m_MiddleLayout;
	QGridLayout			m_RightLayout;
	QGridLayout			m_BottomLeftLayout;
	QGridLayout			m_BottomLayout;
	QGridLayout			m_BottomRightLayout;
	TFView				m_Canvas;
	//QGradientRamp		m_GradientRampDiffuse;
	//QGradientRamp		m_GradientRampSpecular;
	//QGradientRamp		m_GradientRampEmission;
};

}
