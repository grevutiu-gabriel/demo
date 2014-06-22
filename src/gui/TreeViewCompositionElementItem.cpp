#include "TreeViewCompositionElementItem.h"

#include <QMenu>


#include "wrapper/ObjectWrapperMimeData.h"


namespace gui
{

TreeViewCompositionElementItem::TreeViewCompositionElementItem(CompositionElementWrapper::Ptr compositionElementWrapper)
	: QObject(),TreeWidgetItem(),
	  m_compositionElementWrapper(compositionElementWrapper)
{
	setText(0, QString::fromStdString(m_compositionElementWrapper->getElement()->getName()));
	//std::string name = ->getElement()->getName();
	//item->setText(0,QString::fromStdString(name));
	//setText(0, "test");
}

TreeViewCompositionElementItem::~TreeViewCompositionElementItem()
{

}

void TreeViewCompositionElementItem::contextMenu(const QPoint &pos)
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

void TreeViewCompositionElementItem::onCompositionElementAdded(int index)
{
	//m_compositionElementWrapper->getCompositionElement(index);
}

QMimeData *TreeViewCompositionElementItem::mimeData()
{
	return new ObjectWrapperMimeData( m_compositionElementWrapper->getElement()->getElement()->getMetaObject()->getClassName(),std::bind( &TreeViewCompositionElementItem::getObjectWrapper, this ) );
}

ObjectWrapper::Ptr TreeViewCompositionElementItem::getObjectWrapper()
{
	return m_compositionElementWrapper->getElement();
}






}
