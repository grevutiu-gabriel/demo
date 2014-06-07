
#include <QObject>
#include <QTreeWidgetItem>
#include <QPoint>

#include "widgets/TreeWidget/TreeWidgetItem.h"
#include "wrapper/ShotElementWrapper.h"



namespace gui
{



class TreeViewShotElementItem : public QObject, public TreeWidgetItem
{
	Q_OBJECT
public:

	TreeViewShotElementItem( ShotElementWrapper::Ptr shotElementWrapper );
	~TreeViewShotElementItem();


	void contextMenu( const QPoint& pos )override;
	virtual QMimeData* mimeData()override;

	ObjectWrapper::Ptr getObjectWrapper();
public slots:
	void onShotElementAdded( int index );


private:
	ShotElementWrapper::Ptr m_shotElementWrapper;
};





}
