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
	std::cout << "TreeWidgeT::mimeData called!\n" << items.size();
	if(items.size()>0)
	{
		QTreeWidgetItem* qitem = items[0];
		if( dynamic_cast<TreeWidgetItem*>(qitem) )
		{
			std::cout << "TreeWidgeT::got item!\n";
			TreeWidgetItem*item = dynamic_cast<TreeWidgetItem*>(qitem);
			return item->mimeData();
		}
	}
	return QTreeWidget::mimeData(items);
}

}
