#include "TreeViewShotItem.h"

#include <QMenu>





namespace gui
{

TreeViewShotItem::TreeViewShotItem(ShotWrapper::Ptr shotWrapper)
	: QObject(),QTreeWidgetItem(),
	  m_shotWrapper(shotWrapper)
{
	setText(0, QString::fromStdString(m_shotWrapper->getName()));
}

TreeViewShotItem::~TreeViewShotItem()
{

}

void TreeViewShotItem::contextMenu(const QPoint &pos)
{
	QMenu* menu = new QMenu();

	QMenu* createMenu = menu->addMenu("create...");
	createMenu->addAction( "clear" );
	createMenu->addAction( "render texture" );
	createMenu->addAction( "render geometry" );
	createMenu->addAction( "post process" );
	QAction* action = menu->exec(pos);
	if(action)
		std::cout<<action->text().toStdString()<< std::endl;

	delete menu;

}






}
