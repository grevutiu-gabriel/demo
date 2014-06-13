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


		virtual void dragEnterEvent ( QDragEnterEvent * event )override;
		virtual void dragMoveEvent ( QDragMoveEvent * event )override;
		virtual void dropEvent ( QDropEvent * event )override;

		// overides
		QMimeData* mimeData(const QList<QTreeWidgetItem *> items) const override;
	};
}
