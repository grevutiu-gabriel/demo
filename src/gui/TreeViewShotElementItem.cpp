#include "TreeViewShotElementItem.h"

#include <QMenu>


#include "wrapper/ObjectWrapperMimeData.h"


namespace gui
{

TreeViewShotElementItem::TreeViewShotElementItem(ShotElementWrapper::Ptr shotElementWrapper)
	: QObject(),TreeWidgetItem(),
	  m_shotElementWrapper(shotElementWrapper)
{
	setText(0, QString::fromStdString(m_shotElementWrapper->getElement()->getName()));
	//std::string name = ->getElement()->getName();
	//item->setText(0,QString::fromStdString(name));
	//setText(0, "test");
}

TreeViewShotElementItem::~TreeViewShotElementItem()
{

}

void TreeViewShotElementItem::contextMenu(const QPoint &pos)
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

void TreeViewShotElementItem::onShotElementAdded(int index)
{
	//m_shotElementWrapper->getShotElement(index);
}

QMimeData *TreeViewShotElementItem::mimeData()
{
	return new ObjectWrapperMimeData( std::bind( &TreeViewShotElementItem::getObjectWrapper, this ) );
}

ObjectWrapper::Ptr TreeViewShotElementItem::getObjectWrapper()
{
	return m_shotElementWrapper->getElement();
}






}
