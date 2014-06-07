#pragma once


#include <QListWidget>
#include <functional>









namespace gui
{

	class ListWidget : public QListWidget
	{
		Q_OBJECT
	public:
		typedef std::function<QMimeData*(QListWidgetItem*)> GetMimeDataCallback;

		ListWidget();
		~ListWidget();

		GetMimeDataCallback getMimeData;
		// overides
		QMimeData* mimeData(const QList<QListWidgetItem *> items) const override;
	};
}
