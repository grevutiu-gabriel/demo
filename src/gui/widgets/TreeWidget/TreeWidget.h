#pragma once


#include <QTreeWidget>










namespace gui
{

	class TreeWidget : public QTreeWidget
	{
		Q_OBJECT
	public:
		TreeWidget();
		~TreeWidget();

		// overides
		QMimeData* mimeData(const QList<QTreeWidgetItem *> items) const override;
	};
}
