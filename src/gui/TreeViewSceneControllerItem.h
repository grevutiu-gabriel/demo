
#include <QObject>
#include <QTreeWidgetItem>


#include "wrapper/SceneWrapper.h"

#include "widgets/TreeWidget/TreeWidgetItem.h"


namespace gui
{



class TreeViewSceneControllerItem : public QObject, public TreeWidgetItem
{
	Q_OBJECT
public:

	TreeViewSceneControllerItem( SceneWrapper::Ptr sceneWrapper, const std::string& controllerName );
	~TreeViewSceneControllerItem();

	void contextMenu( const QPoint& pos )override;
	virtual QMimeData* mimeData()override;
	ObjectWrapper::Ptr getObjectWrapper();

public slots:


private:
	SceneWrapper::Ptr m_sceneWrapper;
	std::string m_controllerName;
	SceneController::Ptr m_sceneController;
};





}
