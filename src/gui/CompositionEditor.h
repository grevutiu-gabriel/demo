#pragma once

#include <memory>
#include <QObject>

#include "wrapper/CompositionWrapper.h"
#include "UpdateGraphView.h"
#include "PropertyView.h"
#include "Container.h"
namespace gui
{




class CompositionEditor : public QObject
{
	Q_OBJECT
public:
	typedef std::shared_ptr<CompositionEditor> Ptr;
	CompositionEditor( CompositionWrapper::Ptr compositionWrapper );
	~CompositionEditor();
	static Ptr create( CompositionWrapper::Ptr compositionWrapper );

	QWidget* getWidget();
	void updateGuiInfo();

public slots:
	void onGraphSelectionChanged();
	void onPropertyChanged();
	// this callback will be called when an object is being created
	// from within the updategraph. The compositioneditor will check if this
	// is an element and attach it to the composition (for convinience)
	void onObjectCreatedInGraph( ObjectWrapper::Ptr objectWrapper );

private:
	CompositionWrapper::Ptr       m_compositionWrapper;
	QWidget*               m_widget;
	UpdateGraphView::Ptr   m_updateGraphView;
	PropertyView::Ptr      m_propertyView;

};

}
