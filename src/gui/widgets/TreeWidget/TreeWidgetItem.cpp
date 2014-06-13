#include "TreeWidgetItem.h"



#include <iostream>
#include <QDragMoveEvent>





namespace gui
{

	TreeWidgetItem::TreeWidgetItem() : QTreeWidgetItem()
	{
	}

	TreeWidgetItem::~TreeWidgetItem()
	{
	}

	void TreeWidgetItem::dragEnterEvent(QDragEnterEvent *event)
	{
		//std::cout << "TreeWidgetItem dragenter!\n";
		//event->acceptProposedAction();
	}

	void TreeWidgetItem::dragMoveEvent(QDragMoveEvent *event)
	{
		//std::cout << "TreeWidgetItem dragmove!\n";
		//event->acceptProposedAction();
	}

	void TreeWidgetItem::dropEvent(QDropEvent *event)
	{
		//std::cout << "TreeWidgetItem drop!!!!!!!\n";
		//event->acceptProposedAction();
	}

	void TreeWidgetItem::contextMenu(const QPoint &pos)
	{

	}

	QMimeData *TreeWidgetItem::mimeData()
	{
		return 0;
	}
}
