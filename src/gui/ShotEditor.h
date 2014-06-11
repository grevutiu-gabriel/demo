#pragma once

#include <memory>
#include <QObject>

#include "wrapper/ShotWrapper.h"
#include "UpdateGraphView.h"
#include "PropertyView.h"
#include "Container.h"
namespace gui
{




class ShotEditor : public QObject
{
	Q_OBJECT
public:
	typedef std::shared_ptr<ShotEditor> Ptr;
	ShotEditor( ShotWrapper::Ptr shotWrapper );
	~ShotEditor();
	static Ptr create( ShotWrapper::Ptr shotWrapper );

	QWidget* getWidget();

public slots:
	void onGraphSelectionChanged();
	void onPropertyChanged();

private:
	ShotWrapper::Ptr       m_shotWrapper;
	QWidget*               m_widget;
	UpdateGraphView::Ptr   m_updateGraphView;
	PropertyView::Ptr      m_propertyView;

};

}
