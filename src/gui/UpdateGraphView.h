#pragma once

#include <QTreeWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <memory>

#include "wrapper/UpdateGraphWrapper.h"
#include "widgets/qnodeseditor/qnodeseditor.h"
#include "widgets/qnodeseditor/qneblock.h"
#include "widgets/qnodeseditor/qneport.h"
#include "widgets/qnodeseditor/qneconnection.h"




namespace gui
{


class UpdateGraphView : public QNodesEditor
{
	Q_OBJECT
public:
	typedef std::shared_ptr<UpdateGraphView> Ptr;

	UpdateGraphView( UpdateGraphWrapper::Ptr updateGraphWrapper );
	~UpdateGraphView();
	static Ptr create( UpdateGraphWrapper::Ptr updateGraphWrapper );

	QNEBlock* insertNode( ObjectWrapper::Ptr objectWrapper );
	QNEBlock* getNode( ObjectWrapper::Ptr objectWrapper );
	bool hasNode( ObjectWrapper::Ptr objectWrapper );

	virtual bool eventFilter(QObject *object, QEvent *event)override;
	virtual void onConnectionAdded( QNEPort* src, QNEPort* dst )override;
	virtual void onConnectionRemoved( QNEPort* src, QNEPort* dst )override;

	void getSelectedObjects( std::vector<ObjectWrapper::Ptr>& selected );
signals:
	void selectionChanged();
public slots:

public:
	UpdateGraphWrapper::Ptr    m_updateGraphWrapper;

	QGraphicsScene*            m_scene;
	QGraphicsView*             m_view;
	std::map<QNEBlock*, ObjectWrapper::Ptr> m_nodes;
	std::map<quint64, std::string> m_inputs; // register for all inputs
	quint64                    m_nextInput; // counter
	quint64                    m_nextNode; // counter

};


}