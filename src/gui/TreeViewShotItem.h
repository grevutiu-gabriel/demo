
#include <QObject>
#include <QPoint>

#include "widgets/TreeWidget/TreeWidgetItem.h"
#include "wrapper/ShotWrapper.h"



namespace gui
{


class TreeViewShotItem : public QObject, public TreeWidgetItem
{
	Q_OBJECT
public:

	TreeViewShotItem( ShotWrapper::Ptr shotWrapper );
	~TreeViewShotItem();

	virtual void dragEnterEvent( QDragEnterEvent* event )override;
	virtual void dragMoveEvent( QDragMoveEvent* event )override;
	virtual void dropEvent( QDropEvent* event )override;

	virtual void contextMenu( const QPoint& pos )override;
	void doubleClick( int column );
	virtual QMimeData* mimeData()override;
	ObjectWrapper::Ptr getObjectWrapper();
public slots:
	void onShotElementAdded( int index );

private:
	ShotWrapper::Ptr m_shotWrapper;
};





}
