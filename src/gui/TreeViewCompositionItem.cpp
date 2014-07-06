#include "TreeViewCompositionItem.h"

#include <QMenu>


#include "Application.h"
#include "wrapper/ObjectWrapperMimeData.h"

namespace gui
{

TreeViewCompositionItem::TreeViewCompositionItem(CompositionWrapper::Ptr compositionWrapper)
	: QObject(),TreeWidgetItem(),
	  m_compositionWrapper(compositionWrapper)
{
	setText(0, QString::fromStdString(m_compositionWrapper->getName()));

}

TreeViewCompositionItem::~TreeViewCompositionItem()
{

}

void TreeViewCompositionItem::dragEnterEvent(QDragEnterEvent *event)
{
	//std::cout << "TreeViewCompositionItemdrop event!\n";
	event->acceptProposedAction();
}

void TreeViewCompositionItem::dragMoveEvent(QDragMoveEvent *event)
{
	//std::cout << "TreeViewCompositionItemdrop event!\n";
	//event->acceptProposedAction();
}

void TreeViewCompositionItem::dropEvent(QDropEvent *e)
{
//	//std::cout << "TreeViewCompositionItemdrop event!\n";
//	if (e->mimeData()->hasFormat("application/objectwrapper"))
//	{
//		const ObjectWrapperMimeData* md = dynamic_cast<const ObjectWrapperMimeData*>(e->mimeData());
//		if(md)
//		{
//			const MetaObject* moc = ObjectFactory::getMetaObject(md->getClassName());
//			if(moc)
//			{
//				if(ObjectFactory::derivesFrom( moc, "Element" ))
//				{
//					ObjectWrapper::Ptr objectWrapper = md->getObjectWrapper();
//					ElementWrapper::Ptr elementWrapper = std::dynamic_pointer_cast<ElementWrapper>(objectWrapper);
//					if(elementWrapper)
//					{
//						std::cout << "gaga! \n";
//						// yay! add element to composition
//						m_compositionWrapper->addElement(elementWrapper);
//						e->acceptProposedAction();
//					}
//				}
//			}
//		}

//	}
}

void TreeViewCompositionItem::contextMenu(const QPoint &pos)
{
	QMenu* menu = new QMenu();
	menu->addAction("edit...");

//	QMenu* createMenu = menu->addMenu("create...");
//	createMenu->addAction( "Clear" );
//	createMenu->addAction( "RenderTexture" );
//	createMenu->addAction( "RenderGeometry" );
//	createMenu->addAction( "PostProcess" );
	QAction* action = menu->exec(pos);
	if(action)
		if(action->text() == "edit...")
		{
			gui::Application::getInstance()->openCompositionEditor( m_compositionWrapper );
			//QString elementTypeName = action->text().toStdString();
			//std::cout<<<< std::endl;
		}

	delete menu;

}

void TreeViewCompositionItem::doubleClick(int column)
{
	gui::Application::getInstance()->openCompositionEditor( m_compositionWrapper );
}

QMimeData *TreeViewCompositionItem::mimeData()
{
	return new ObjectWrapperMimeData( m_compositionWrapper->getComposition()->getMetaObject()->getClassName(),std::bind( &TreeViewCompositionItem::getObjectWrapper, this ) );
}

ObjectWrapper::Ptr TreeViewCompositionItem::getObjectWrapper()
{
	return m_compositionWrapper;
}








}
