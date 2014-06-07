#include "UpdateGraphView.h"

#include "wrapper/ObjectWrapperMimeData.h"

#include <QVBoxLayout>
#include <QToolBar>
#include <QFileDialog>
#include <QEvent>
#include <QDropEvent>
#include <QGraphicsSceneDragDropEvent>

#include "Application.h"

namespace gui
{



UpdateGraphView::Ptr UpdateGraphView::create( UpdateGraphWrapper::Ptr updateGraphWrapper)
{
	return std::make_shared<UpdateGraphView>( updateGraphWrapper );
}

QNEBlock* UpdateGraphView::insertNode(ObjectWrapper::Ptr objectWrapper)
{
	QString name = QString::fromStdString(objectWrapper->getName());
	QString type = QString::fromStdString(objectWrapper->getObject()->getMetaObject()->getTypeName());
	if( name.isEmpty() )
		name = "unnamed";
	QNEBlock *b = new QNEBlock(0);
	m_scene->addItem(b);

	// name
	b->addPort(name, 0, QNEPort::NamePort);
	// type
	b->addPort(type, 0, QNEPort::TypePort);

	// inputs (properties)
	std::vector<std::string> propNames;
	objectWrapper->getObject()->getPropertyNames(propNames);
	for(auto propName:propNames)
	{
		m_inputs[m_nextInput] = propName;
		QNEPort*p = b->addInputPort(QString::fromStdString(propName));
		p->setPtr(m_nextInput++);
	}

	// outputs (controller)
	if( std::dynamic_pointer_cast<Controller>(objectWrapper->getObject()) )
	{
		b->addOutputPort("");
	}

	// keep track of added nodes
	m_nodes[b] = objectWrapper;
	b->setPtr(m_nextNode);
	m_nextNode++;

	return b;
}

QNEBlock *UpdateGraphView::getNode(ObjectWrapper::Ptr objectWrapper)
{
	for( auto it:m_nodes )
		if(objectWrapper==it.second)
			return it.first;
	return 0;
}

bool UpdateGraphView::hasNode(ObjectWrapper::Ptr objectWrapper)
{
	for( auto it:m_nodes )
	{
		if(it.second == objectWrapper)
			return true;
	}
	return false;
}

bool UpdateGraphView::eventFilter(QObject *object, QEvent *event)
{
	switch ((int) event->type())
	{
	case QEvent::GraphicsSceneDragEnter:
	{
		QGraphicsSceneDragDropEvent* e = dynamic_cast<QGraphicsSceneDragDropEvent*>(event);
		if (e->mimeData()->hasFormat("application/objectwrapper"))
			e->acceptProposedAction();
		return true;
	}break;
	case QEvent::GraphicsSceneDragMove:
	{
		QGraphicsSceneDragDropEvent* e = dynamic_cast<QGraphicsSceneDragDropEvent*>(event);
		if (e->mimeData()->hasFormat("application/objectwrapper"))
			e->acceptProposedAction();
		return true;
	}break;
	case QEvent::GraphicsSceneDrop:
	{
		//std::cout << "drop\n";
		QGraphicsSceneDragDropEvent* e = dynamic_cast<QGraphicsSceneDragDropEvent*>(event);
		if (e->mimeData()->hasFormat("application/objectwrapper"))
		{
			//std::cout << "drop---hasFormat\n";
			const ObjectWrapperMimeData* md = dynamic_cast<const ObjectWrapperMimeData*>(e->mimeData());
			if(md && !hasNode(md->getObjectWrapper()))
			{
				//std::cout << "drop---hasFormat---!hasNode+md\n";
				QNEBlock* b = insertNode( md->getObjectWrapper() );
				b->setPos( e->scenePos() );
			}
			e->acceptProposedAction();
		}
		return true;
	}break;
	};

	return QNodesEditor::eventFilter(object, event);
}

void UpdateGraphView::onConnectionAdded(QNEPort *src, QNEPort *dst)
{
	ObjectWrapper::Ptr controller = m_nodes[src->block()];
	ObjectWrapper::Ptr object = m_nodes[dst->block()];
	std::string propName = m_inputs[dst->ptr()];
	m_updateGraphWrapper->addConnection( controller, object, propName );
	Application::getInstance()->getGlViewer()->update();
}

void UpdateGraphView::onConnectionRemoved(QNEPort *src, QNEPort *dst)
{
	ObjectWrapper::Ptr controller = m_nodes[src->block()];
	ObjectWrapper::Ptr object = m_nodes[dst->block()];
	std::string propName = m_inputs[dst->ptr()];
	m_updateGraphWrapper->removeConnection( controller, object, propName );
	Application::getInstance()->getGlViewer()->update();
}

UpdateGraphView::UpdateGraphView(UpdateGraphWrapper::Ptr updateGraphWrapper) :
	QNodesEditor(),
	m_updateGraphWrapper(updateGraphWrapper),
	m_nextInput(0),
	m_nextNode(0)
{

	m_scene = new QGraphicsScene();
	m_view = new QGraphicsView();
	m_view->setScene(m_scene);
	m_nodeEditor = this;
	//m_nodeEditor = new QNodesEditor();
	m_nodeEditor->install(m_scene);


	std::vector<ObjectWrapper::Ptr> nodes;
	m_updateGraphWrapper->getNodes(nodes);
	int index = 0;
	for( auto node:nodes )
	{
		QNEBlock* b = insertNode( node );

		b->setPos(150, index*150);
		++index;
	}

	std::vector<UpdateGraph::Connection> connections;
	m_updateGraphWrapper->getUpdateGraph()->getConnections(connections);
	for(auto conn:connections)
	{
		QNEBlock* srcBlock = getNode(Application::getInstance()->getWrapper(conn.src));
		QVector<QNEPort*> srcPorts = srcBlock->ports();
		QNEPort* srcPort = 0;
		for(auto p:srcPorts)
			if(p->isOutput())
			{
				srcPort = p;
				break;
			}
		QNEBlock* destBlock = getNode(Application::getInstance()->getWrapper(conn.dest));
		QNEPort* destPort = destBlock->getPort(conn.propName);
		QNEConnection* path = new QNEConnection(0);
		m_scene->addItem(path);
		path->setPort1(srcPort);
		path->setPort2(destPort);
		path->setPos1(srcPort->scenePos());
		path->setPos2(destPort->scenePos());
		path->updatePath();
	}


}

UpdateGraphView::~UpdateGraphView()
{
	//delete m_treeWidget;
}


}
