#include "TreeViewSceneItem.h"

#include "TreeViewSceneControllerItem.h"

#include "wrapper/ObjectWrapperMimeData.h"



namespace gui
{

TreeViewSceneItem::TreeViewSceneItem(SceneWrapper::Ptr sceneWrapper)
	: QObject(),TreeWidgetItem(),
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
		TreeViewSceneControllerItem* item = new TreeViewSceneControllerItem( m_sceneWrapper, name );
		this->addChild(item);
	}

}

void TreeViewSceneItem::onSceneReloaded()
{
	update();
}

void TreeViewSceneItem::contextMenu(const QPoint &pos)
{
//	QMenu* menu = new QMenu();

//	QMenu* createMenu = menu->addMenu("create...");
//	createMenu->addAction( "clear" );
//	createMenu->addAction( "render texture" );
//	createMenu->addAction( "render geometry" );
//	createMenu->addAction( "post process" );
//	QAction* action = menu->exec(pos);
//	if(action)
//		std::cout<<action->text().toStdString()<< std::endl;

//	delete menu;

}

QMimeData *TreeViewSceneItem::mimeData()
{
	return new ObjectWrapperMimeData( m_sceneWrapper );
}







}
