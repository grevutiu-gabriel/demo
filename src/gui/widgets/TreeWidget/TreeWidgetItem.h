#pragma once
#include <QObject>
#include <QTreeWidgetItem>
#include <QPoint>




namespace gui
{



class TreeWidgetItem : public QTreeWidgetItem
{
public:

	TreeWidgetItem();
	~TreeWidgetItem();


	virtual void contextMenu( const QPoint& pos );
	virtual QMimeData* mimeData();
public slots:
private:
};





}
