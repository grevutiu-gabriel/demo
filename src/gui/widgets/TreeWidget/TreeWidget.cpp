#include "TreeWidget.h"
#include "TreeWidgetItem.h"

#include <iostream>


#include <QDragEnterEvent>





namespace gui
{

TreeWidget::TreeWidget() : QTreeWidget()
{

}

TreeWidget::~TreeWidget()
{

}

void TreeWidget::dragEnterEvent(QDragEnterEvent *event)
{
	std::cout << "treewidget::dragenter\n";
//	QTreeWidgetItem* qitem = itemAt(event->pos());
//	if (!qitem)
//		return;

//	if( dynamic_cast<TreeWidgetItem*>(qitem) )
//	{
//		TreeWidgetItem* item = dynamic_cast<TreeWidgetItem*>(qitem);
//		item->dragEnterEvent(event);
//	}
	event->acceptProposedAction();
}

void TreeWidget::dragMoveEvent(QDragMoveEvent *event)
{
	QTreeWidgetItem* qitem = itemAt(event->pos());

	if (!qitem)
		return;

	if( dynamic_cast<TreeWidgetItem*>(qitem) )
	{
		TreeWidgetItem* item = dynamic_cast<TreeWidgetItem*>(qitem);
		item->dragMoveEvent(event);
	}
}

void TreeWidget::dropEvent(QDropEvent *event)
{
	QTreeWidgetItem* qitem = itemAt(event->pos());
	if (!qitem)
		return;

	if( dynamic_cast<TreeWidgetItem*>(qitem) )
	{
		TreeWidgetItem* item = dynamic_cast<TreeWidgetItem*>(qitem);
		item->dropEvent(event);
	}
}

QMimeData *TreeWidget::mimeData(const QList<QTreeWidgetItem *> items) const
{
	if(items.size()>0)
	{
		QTreeWidgetItem* qitem = items[0];
		if( dynamic_cast<TreeWidgetItem*>(qitem) )
		{
			TreeWidgetItem*item = dynamic_cast<TreeWidgetItem*>(qitem);
			return item->mimeData();
		}
	}
	return QTreeWidget::mimeData(items);
}

}
