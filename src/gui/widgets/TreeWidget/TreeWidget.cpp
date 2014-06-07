#include "TreeWidget.h"
#include "TreeWidgetItem.h"

#include <iostream>








namespace gui
{

TreeWidget::TreeWidget() : QTreeWidget()
{

}

TreeWidget::~TreeWidget()
{

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
