#include "TreeViewShotItem.h"

#include "TreeViewShotElementItem.h"
#include <QMenu>


#include "Application.h"
#include "wrapper/ObjectWrapperMimeData.h"

namespace gui
{

TreeViewShotItem::TreeViewShotItem(ShotWrapper::Ptr shotWrapper)
	: QObject(),TreeWidgetItem(),
	  m_shotWrapper(shotWrapper)
{
	setText(0, QString::fromStdString(m_shotWrapper->getName()));

	connect( m_shotWrapper.get(), SIGNAL(shotElementAdded(int)), this, SLOT(onShotElementAdded(int)) );
}

TreeViewShotItem::~TreeViewShotItem()
{

}

void TreeViewShotItem::dragEnterEvent(QDragEnterEvent *event)
{
	//std::cout << "TreeViewShotItemdrop event!\n";
	event->acceptProposedAction();
}

void TreeViewShotItem::dragMoveEvent(QDragMoveEvent *event)
{
	//std::cout << "TreeViewShotItemdrop event!\n";
	//event->acceptProposedAction();
}

void TreeViewShotItem::dropEvent(QDropEvent *e)
{
	//std::cout << "TreeViewShotItemdrop event!\n";
	if (e->mimeData()->hasFormat("application/objectwrapper"))
	{
		const ObjectWrapperMimeData* md = dynamic_cast<const ObjectWrapperMimeData*>(e->mimeData());
		if(md)
		{
			const MetaObject* moc = ObjectFactory::getMetaObject(md->getClassName());
			if(moc)
			{
				if(ObjectFactory::derivesFrom( moc, "Element" ))
				{
					ObjectWrapper::Ptr objectWrapper = md->getObjectWrapper();
					ElementWrapper::Ptr elementWrapper = std::dynamic_pointer_cast<ElementWrapper>(objectWrapper);
					if(elementWrapper)
					{
						std::cout << "gaga! \n";
						// yay! add element to shot
						m_shotWrapper->addElement(elementWrapper);
						e->acceptProposedAction();
					}
				}
			}
		}

	}
}

void TreeViewShotItem::contextMenu(const QPoint &pos)
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
			gui::Application::getInstance()->openShotEditor( m_shotWrapper );
			//QString elementTypeName = action->text().toStdString();
			//std::cout<<<< std::endl;
		}

	delete menu;

}

void TreeViewShotItem::doubleClick(int column)
{
	gui::Application::getInstance()->openShotEditor( m_shotWrapper );
}

QMimeData *TreeViewShotItem::mimeData()
{
	return new ObjectWrapperMimeData( m_shotWrapper->getShot()->getMetaObject()->getClassName(),std::bind( &TreeViewShotItem::getObjectWrapper, this ) );
}

ObjectWrapper::Ptr TreeViewShotItem::getObjectWrapper()
{
	return m_shotWrapper;
}

void TreeViewShotItem::onShotElementAdded(int index)
{
	TreeViewShotElementItem* item = new TreeViewShotElementItem(m_shotWrapper->getShotElement(index));
	this->addChild(item);
}






}
