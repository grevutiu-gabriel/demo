#include "ListWidget.h"


#include <iostream>








namespace gui
{

ListWidget::ListWidget() : QListWidget()
{

}

ListWidget::~ListWidget()
{

}

QMimeData *ListWidget::mimeData(const QList<QListWidgetItem *> items) const
{
	if(items.size()>0)
	{
		QListWidgetItem* qitem = items[0];
		if(getMimeData)
			return getMimeData(qitem);
	}
	return QListWidget::mimeData(items);
}

}
