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
	void onCompositionAdded( int index );
	void load();
	void save();
	void loadScene();
	void newComposition();
	void onCustomContextMenuRequested ( const QPoint & pos );
	void onItemDoubleClicked(QTreeWidgetItem * item, int column);
public:
	QWidget*            m_widget;
	TreeWidget*         m_treeWidget;
	DemoWrapper::Ptr    m_demoWrapper;
};


}
