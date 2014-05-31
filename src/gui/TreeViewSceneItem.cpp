#include "TreeViewSceneItem.h"







namespace gui
{

TreeViewSceneItem::TreeViewSceneItem(SceneWrapper::Ptr sceneWrapper)
	: QObject(),QTreeWidgetItem(),
	  m_sceneWrapper(sceneWrapper)
{
	setText(0, QString::fromStdString(m_sceneWrapper->getName()));
	update();
	//connect( m_sceneWrapper.get(), SIGNAL(sceneReloaded()), this, SLOT(onSceneReloaded()) );
	connect(m_sceneWrapper.get(), SIGNAL(sceneReloaded()), this, SLOT(onSceneReloaded()));
}

TreeViewSceneItem::~TreeViewSceneItem()
{

}

void TreeViewSceneItem::update()
{
	// clear all children ---
	QList<QTreeWidgetItem*> childs = this->takeChildren();
	for( auto it=childs.begin(),end=childs.end();it!=end;++it )
	{
		QTreeWidgetItem* child = *it;
		delete child;
	}
	childs.clear();

	// explore scene controller and add as children ---
	std::vector<std::string> controllerNames;
	m_sceneWrapper->getControllerNames(controllerNames);

	for(auto name:controllerNames)
	{
		QTreeWidgetItem* item = new QTreeWidgetItem();
		item->setText(0,QString::fromStdString(name));
		this->addChild(item);
	}

}

void TreeViewSceneItem::onSceneReloaded()
{
	update();
}









}
