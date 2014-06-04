#include "UpdateGraphView.h"

#include "wrapper/ObjectWrapperMimeData.h"

#include <QVBoxLayout>
#include <QToolBar>
#include <QFileDialog>
#include <QEvent>
#include <QDropEvent>
#include <QGraphicsSceneDragDropEvent>


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
	m_nodes[m_nextNode] = objectWrapper;
	b->setPtr(m_nextNode);
	m_nextNode++;

	return b;
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
	std::cout << "connection added !\n";
	ObjectWrapper::Ptr controller = m_nodes[src->block()->ptr()];
	ObjectWrapper::Ptr object = m_nodes[dst->block()->ptr()];
	std::string propName = m_inputs[dst->ptr()];
	m_updateGraphWrapper->addConnection( controller, object, propName );
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

//		b->addOutputPort("out1");
//		b->addOutputPort("out2");
//		b->addOutputPort("out3");
		b->setPos(150, index*150);
		++index;
	}




}

UpdateGraphView::~UpdateGraphView()
{
	//delete m_treeWidget;
}


}
