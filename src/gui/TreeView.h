#pragma once

#include <QTreeWidget>
#include <memory>

#include "gui/widgets/TreeWidget/TreeWidget.h"
#include "wrapper/DemoWrapper.h"






namespace gui
{


class TreeView : QObject
{
	Q_OBJECT
public:
	typedef std::shared_ptr<TreeView> Ptr;

	TreeView( DemoWrapper::Ptr demoWrapper );
	~TreeView();
	static Ptr create( DemoWrapper::Ptr demoWrapper );





public slots:
	void onSceneAdded( int index );
	void onShotAdded( int index );
	void loadScene();
	void newShot();
	void onCustomContextMenuRequested ( const QPoint & pos );
public:
	QWidget*            m_widget;
	TreeWidget*         m_treeWidget;
	DemoWrapper::Ptr    m_demoWrapper;
};


}
