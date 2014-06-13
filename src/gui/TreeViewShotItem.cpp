#include "TreeViewShotItem.h"

#include "TreeViewShotElementItem.h"
#include <QMenu>


#include "Application.h"


namespace gui
{

TreeViewShotItem::TreeViewShotItem(ShotWrapper::Ptr shotWrapper)
	: QObject(),QTreeWidgetItem(),
	  m_shotWrapper(shotWrapper)
{
	setText(0, QString::fromStdString(m_shotWrapper->getName()));

	connect( m_shotWrapper.get(), SIGNAL(shotElementAdded(int)), this, SLOT(onShotElementAdded(int)) );
}

TreeViewShotItem::~TreeViewShotItem()
{

}

void TreeViewShotItem::contextMenu(const QPoint &pos)
{
	QMenu* menu = new QMenu();
	menu->addAction("open editor...");

//	QMenu* createMenu = menu->addMenu("create...");
//	createMenu->addAction( "Clear" );
//	createMenu->addAction( "RenderTexture" );
//	createMenu->addAction( "RenderGeometry" );
//	createMenu->addAction( "PostProcess" );
	QAction* action = menu->exec(pos);
	if(action->text() == "open editor...")
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

void TreeViewShotItem::onShotElementAdded(int index)
{
	TreeViewShotElementItem* item = new TreeViewShotElementItem(m_shotWrapper->getShotElement(index));
	this->addChild(item);
}






}
