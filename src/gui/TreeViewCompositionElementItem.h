
#include <QObject>
#include <QTreeWidgetItem>
#include <QPoint>

#include "widgets/TreeWidget/TreeWidgetItem.h"
#include "wrapper/CompositionElementWrapper.h"



namespace gui
{



class TreeViewCompositionElementItem : public QObject, public TreeWidgetItem
{
	Q_OBJECT
public:

	TreeViewCompositionElementItem( CompositionElementWrapper::Ptr compositionElementWrapper );
	~TreeViewCompositionElementItem();


	void contextMenu( const QPoint& pos )override;
	virtual QMimeData* mimeData()override;

	ObjectWrapper::Ptr getObjectWrapper();
public slots:
	void onCompositionElementAdded( int index );


private:
	CompositionElementWrapper::Ptr m_compositionElementWrapper;
};





}
