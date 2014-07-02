#include "UpdateGraphView.h"

#include "wrapper/ObjectWrapperMimeData.h"

#include <QVBoxLayout>
#include <QToolBar>
#include <QFileDialog>
#include <QEvent>
#include <QDropEvent>
#include <QGraphicsSceneDragDropEvent>

#include "Application.h"

#include "../framework/ListProperty.h"

namespace gui
{



UpdateGraphView::Ptr UpdateGraphView::create( UpdateGraphWrapper::Ptr updateGraphWrapper)
{
	return std::make_shared<UpdateGraphView>( updateGraphWrapper );
}

QNEBlock* UpdateGraphView::insertNode(ObjectWrapper::Ptr objectWrapper)
{
	QString name = QString::fromStdString(objectWrapper->getName());
	QString type = QString::fromStdString(objectWrapper->getObject()->getMetaObject()->getClassName());
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
		QNEPort*p = b->addInputPort(QString::fromStdString(propName));
		m_inputs[p] = propName;
		//p->setPtr(m_nextInput++);
	}

	// inputs (propertygroups - especially lists)
	std::vector<PropertyGroup::Ptr> propGroups;
	objectWrapper->getObject()->getPropertyGroups(propGroups);
	for( auto propGroup:propGroups )
	{
		if( std::dynamic_pointer_cast<ListProperty>(propGroup) )
		{
			std::string groupName = propGroup->getName();
			// we have a list ... add special port for adding new items
			QString name = QString::fromStdString("new item (" + propGroup->getName() + ")");
			//QNEPort*p = b->addPort(name, false, QNEPort::ActionPort);
			QNEPort*p = b->addInputPort(name);
			m_groups[p] = groupName;
		}
	}


	// outputs (controller)
	if( std::dynamic_pointer_cast<Controller>(objectWrapper->getObject()) )
	{
		b->addOutputPort("");
	}

	// keep track of added nodes
	m_nodes[b] = objectWrapper;
	//b->setPtr(m_nextNode);
	//m_nextNode++;

	m_updateGraphWrapper->addNode( objectWrapper );

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
			ObjectWrapper::Ptr objectWrapper = md->getObjectWrapper();
			if(md && !hasNode(objectWrapper))
			{
				//std::cout << "drop---hasFormat---!hasNode+md\n";
				QNEBlock* b = insertNode( objectWrapper );
				b->setPos( e->scenePos() );
			}

			if( e->mimeData()->hasFormat("application/createobject") )
			{
				// emit objectcreated..
				emit objectCreated(objectWrapper);
			}

			e->acceptProposedAction();
		}
		return true;
	}break;
	};

	return QNodesEditor::eventFilter(object, event);
}

void UpdateGraphView::onConnectionAdded(QNEPort *src, QNEPort *dst, QNEConnection* conn)
{
	ObjectWrapper::Ptr controller = m_nodes[src->block()];
	ObjectWrapper::Ptr object = m_nodes[dst->block()];


	auto it = m_inputs.find(dst);
	auto git = m_groups.find(dst);
	// if port is associated with a property...
	if( it!=m_inputs.end() )
	{
		// get the property and add connection
		std::string propName = it->second;
		m_updateGraphWrapper->addConnection( controller, object, propName );
	}else
	// see if port is associated with a group
	if( git != m_groups.end() )
	{
		// remove connection
		delete conn;

		// remove the port
		QNEBlock* block = dst->block();
		block->removePort( dst->portName() );

		// get the propertygroup
		std::string propGroupName = git->second;
		PropertyGroup::Ptr propGroup = object->getObject()->getPropertyGroup( propGroupName );
		// see if propGroup is a list
		ListProperty::Ptr list = std::dynamic_pointer_cast<ListProperty>( propGroup );
		if(list)
		{
			// then this means adding a new item...
			Property::Ptr prop = object->addProperty( list->getName() );
			std::string propName = prop->getName();

			// add connection
			QNEConnection* conn2 = new QNEConnection(0);
			m_scene->addItem(conn2);
			conn2->setPort1(src);
			conn2->setPort2(block->getPort(QString::fromStdString(propName)));
			conn2->updatePath();
			m_updateGraphWrapper->addConnection( controller, object, propName );

			// add new item port again
			block->addPort(dst);
		}

	}


	Application::getInstance()->getGlViewer()->update();

}

void UpdateGraphView::onConnectionRemoved(QNEPort *src, QNEPort *dst)
{
	ObjectWrapper::Ptr controller = m_nodes[src->block()];
	ObjectWrapper::Ptr object = m_nodes[dst->block()];
	std::string propName = m_inputs[dst];
	m_updateGraphWrapper->removeConnection( controller, object, propName );
	Application::getInstance()->getGlViewer()->update();
}

void UpdateGraphView::getSelectedObjects(std::vector<ObjectWrapper::Ptr> &selected)
{
	selected.clear();
	QList<QGraphicsItem *>& selectedItems = m_scene->selectedItems();
	for( auto selectedItem:selectedItems )
	{
		QNEBlock* block = dynamic_cast<QNEBlock*>(selectedItem);
		if(block)
		{
			auto it = m_nodes.find(block);
			if(it!=m_nodes.end())
				selected.push_back(it->second);
		}
	}
}

void UpdateGraphView::updateGuiInfo()
{
	for( auto it:m_nodes )
	{
		QNEBlock* block = it.first;
		ObjectWrapper::Ptr wrapper = it.second;

		m_updateGraphWrapper->setPosition( wrapper, block->pos() );
	}
}

void UpdateGraphView::onObjectPropertyAdded(ObjectWrapper::Ptr objectWrapper, const std::string &propName)
{
	QNEBlock* block = getNode(objectWrapper);
	if( block )
	{
		std::cout << "UpdateGraphView::onObjectPropertyAdded: " << propName << std::endl;
		std::cout << "obj name: " << objectWrapper->getName() << std::endl;
		QNEPort* port = block->addInputPort( QString::fromStdString(propName) );
		m_inputs[port] = propName;
	}
}

void UpdateGraphView::onObjectPropertyRemoved(ObjectWrapper::Ptr objectWrapper, const std::string &propName)
{
	std::cout << "UpdateGraphView::onObjectPropertyRemoved: " << propName << std::endl;
}

UpdateGraphView::UpdateGraphView(UpdateGraphWrapper::Ptr updateGraphWrapper) :
	QNodesEditor(),
	m_updateGraphWrapper(updateGraphWrapper)
{

	m_scene = new QGraphicsScene();
	m_view = new QGraphicsView();
	m_view->setScene(m_scene);
	install(m_scene);


	std::vector<ObjectWrapper::Ptr> nodes;
	m_updateGraphWrapper->getNodes(nodes);
	int index = 0;
	for( auto node:nodes )
	{
		QNEBlock* b = insertNode( node );

		b->setPos(m_updateGraphWrapper->getPosition(node));
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
		QNEPort* destPort = destBlock->getPort(QString::fromStdString(conn.propName));
		QNEConnection* path = new QNEConnection(0);
		m_scene->addItem(path);
		path->setPort1(srcPort);
		path->setPort2(destPort);
		path->setPos1(srcPort->scenePos());
		path->setPos2(destPort->scenePos());
		path->updatePath();
	}

	connect( m_scene, SIGNAL(selectionChanged()), this, SIGNAL(selectionChanged()) );
	connect( m_updateGraphWrapper.get(), SIGNAL(propertyAdded(ObjectWrapper::Ptr,const std::string&)), this, SLOT(onObjectPropertyAdded(ObjectWrapper::Ptr,std::string)) );
	connect( m_updateGraphWrapper.get(), SIGNAL(propertyRemoved(ObjectWrapper::Ptr,const std::string&)), this, SLOT(onObjectPropertyRemoved(ObjectWrapper::Ptr,std::string)) );

}

UpdateGraphView::~UpdateGraphView()
{
	//delete m_treeWidget;
}


}
