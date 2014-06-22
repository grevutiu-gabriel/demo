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
	std::cout << "ffffffffffff\n";
	QMenu* menu = new QMenu();

//	QMenu* createMenu = menu->addMenu("create...");
//	createMenu->addAction( "clear" );
//	createMenu->addAction( "render texture" );
//	createMenu->addAction( "render geometry" );
//	createMenu->addAction( "post process" );
	menu->addAction( "move up" );
	menu->addAction( "move down" );
	QAction* action = menu->exec(pos);
	if(action)
	{
		if( action->text() == "move up" )
		{
			QTreeWidgetItem* item = this;
			if (item)
			{
				QTreeWidgetItem* parent = item->parent();
				int index = parent->indexOfChild(item);
				QTreeWidgetItem* child = parent->takeChild(index);
				parent->insertChild(index-1, child);
				parent->setExpanded(true);
				child->setExpanded(true);
			}
		}else
		if( action->text() == "move down" )
		{
		}
		//std::cout<<action->text().toStdString()<< std::endl;
	}
	delete menu;

}

void TreeViewShotElementItem::onShotElementAdded(int index)
{
	//m_shotElementWrapper->getShotElement(index);
}

QMimeData *TreeViewShotElementItem::mimeData()
{
	return new ObjectWrapperMimeData( m_shotElementWrapper->getElement()->getElement()->getMetaObject()->getClassName(),std::bind( &TreeViewShotElementItem::getObjectWrapper, this ) );
}

ObjectWrapper::Ptr TreeViewShotElementItem::getObjectWrapper()
{
	return m_shotElementWrapper->getElement();
}






}
