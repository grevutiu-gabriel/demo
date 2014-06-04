#pragma once

#include <QTreeWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <memory>

#include "wrapper/UpdateGraphWrapper.h"
#include "widgets/qnodeseditor/qnodeseditor.h"
#include "widgets/qnodeseditor/qneblock.h"
#include "widgets/qnodeseditor/qneport.h"




namespace gui
{


class UpdateGraphView : QNodesEditor
{
	Q_OBJECT
public:
	typedef std::shared_ptr<UpdateGraphView> Ptr;

	UpdateGraphView( UpdateGraphWrapper::Ptr updateGraphWrapper );
	~UpdateGraphView();
	static Ptr create( UpdateGraphWrapper::Ptr updateGraphWrapper );

	QNEBlock* insertNode( ObjectWrapper::Ptr objectWrapper );
	bool hasNode( ObjectWrapper::Ptr objectWrapper );

	virtual bool eventFilter(QObject *object, QEvent *event)override;
	virtual void onConnectionAdded( QNEPort* src, QNEPort* dst )override;

public slots:

public:
	UpdateGraphWrapper::Ptr    m_updateGraphWrapper;

	QNodesEditor*              m_nodeEditor;
	QGraphicsScene*            m_scene;
	QGraphicsView*             m_view;
	std::map<quint64, ObjectWrapper::Ptr> m_nodes;
	std::map<quint64, std::string> m_inputs; // register for all inputs
	quint64                    m_nextInput; // counter
	quint64                    m_nextNode; // counter

};


}
