#include "TreeViewSceneControllerItem.h"



#include "wrapper/ObjectWrapperMimeData.h"

#include "Application.h"

namespace gui
{

TreeViewSceneControllerItem::TreeViewSceneControllerItem(SceneWrapper::Ptr sceneWrapper, const std::string& controllerName)
	: QObject(),TreeWidgetItem(),
	  m_sceneWrapper(sceneWrapper),
	  m_controllerName(controllerName)
{
	setText(0, QString::fromStdString(m_controllerName));
}

TreeViewSceneControllerItem::~TreeViewSceneControllerItem()
{

}


void TreeViewSceneControllerItem::contextMenu(const QPoint &pos)
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

QMimeData *TreeViewSceneControllerItem::mimeData()
{
	return new ObjectWrapperMimeData( std::bind( &TreeViewSceneControllerItem::getObjectWrapper, this ) );
}

ObjectWrapper::Ptr TreeViewSceneControllerItem::getObjectWrapper()
{
	// create controller if it hasnt been created yet
	if(!m_sceneController)
	{
		m_sceneController = SceneController::create(m_sceneWrapper->getScene(),m_controllerName);
		m_sceneController->setName(m_controllerName);
	}
	return Application::getInstance()->getWrapper(m_sceneController);
}







}
