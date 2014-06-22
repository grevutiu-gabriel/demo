
#include <QObject>
#include <QPoint>

#include "widgets/TreeWidget/TreeWidgetItem.h"
#include "wrapper/CompositionWrapper.h"



namespace gui
{


class TreeViewCompositionItem : public QObject, public TreeWidgetItem
{
	Q_OBJECT
public:

	TreeViewCompositionItem( CompositionWrapper::Ptr compositionWrapper );
	~TreeViewCompositionItem();

	virtual void dragEnterEvent( QDragEnterEvent* event )override;
	virtual void dragMoveEvent( QDragMoveEvent* event )override;
	virtual void dropEvent( QDropEvent* event )override;

	virtual void contextMenu( const QPoint& pos )override;
	void doubleClick( int column );
	virtual QMimeData* mimeData()override;
	ObjectWrapper::Ptr getObjectWrapper();
public slots:
	void onCompositionElementAdded( int index );
	void onCompositionElementRemoved( int index );

private:
	CompositionWrapper::Ptr m_compositionWrapper;
};





}
