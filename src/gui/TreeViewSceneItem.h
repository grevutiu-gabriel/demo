
#include <QObject>
#include <QTreeWidgetItem>


#include "wrapper/SceneWrapper.h"



namespace gui
{



class TreeViewSceneItem : public QObject, public QTreeWidgetItem
{
	Q_OBJECT
public:

	TreeViewSceneItem( SceneWrapper::Ptr sceneWrapper );
	~TreeViewSceneItem();


	void update();


public slots:
	void onSceneReloaded();


private:
	SceneWrapper::Ptr m_sceneWrapper;
};





}
