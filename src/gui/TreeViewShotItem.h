
#include <QObject>
#include <QTreeWidgetItem>
#include <QPoint>


#include "wrapper/ShotWrapper.h"



namespace gui
{



class TreeViewShotItem : public QObject, public QTreeWidgetItem
{
	Q_OBJECT
public:

	TreeViewShotItem( ShotWrapper::Ptr shotWrapper );
	~TreeViewShotItem();


	void contextMenu( const QPoint& pos );

public slots:
	void onShotElementAdded( int index );

private:
	ShotWrapper::Ptr m_shotWrapper;
};





}
