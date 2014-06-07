
#include <QObject>
#include <QTreeWidgetItem>


#include "wrapper/SceneWrapper.h"

#include "widgets/TreeWidget/TreeWidgetItem.h"


namespace gui
{



class TreeViewSceneItem : public QObject, public TreeWidgetItem
{
	Q_OBJECT
public:

	TreeViewSceneItem( SceneWrapper::Ptr sceneWrapper );
	~TreeViewSceneItem();

	void contextMenu( const QPoint& pos )override;
	virtual QMimeData* mimeData()override;
	void update();

	ObjectWrapper::Ptr getObjectWrapper();

public slots:
	void onSceneReloaded();


private:
	SceneWrapper::Ptr m_sceneWrapper;
};





}
