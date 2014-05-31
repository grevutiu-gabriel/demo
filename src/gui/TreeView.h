#pragma once

#include <QTreeWidget>
#include <memory>

#include "wrapper/DemoWrapper.h"






namespace gui
{


class TreeView : QObject
{
	Q_OBJECT
public:
	typedef std::shared_ptr<TreeView> Ptr;

	TreeView( DemoWrapper::Ptr demoWrapper );

	static Ptr create( DemoWrapper::Ptr demoWrapper );



	TreeView();
	~TreeView();
public slots:
	void onSceneAdded( int index );
	void loadScene();
public:
	QWidget*            m_widget;
	QTreeWidget*        m_treeWidget;
	DemoWrapper::Ptr    m_demoWrapper;
};


}
