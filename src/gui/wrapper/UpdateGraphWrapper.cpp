#include "UpdateGraphWrapper.h"

#include "../Application.h"

namespace gui
{

	UpdateGraphWrapper::UpdateGraphWrapper( UpdateGraph::Ptr graph )
		:QObject(),
		m_graph(graph)
	{
	}

	UpdateGraphWrapper::Ptr UpdateGraphWrapper::create(UpdateGraph::Ptr graph)
	{
		return std::make_shared<UpdateGraphWrapper>(graph);
	}

	UpdateGraph::Ptr UpdateGraphWrapper::getUpdateGraph()
	{
		return m_graph;
	}

	void UpdateGraphWrapper::load()
	{
		std::vector<Object::Ptr> nodes;
		m_graph->getNodes(nodes);

		for( auto node:nodes )
		{
			ObjectWrapper::Ptr wrapper = Application::getInstance()->getWrapper(node);
			addNode(wrapper);
		}
	}

	void UpdateGraphWrapper::getNodes( std::vector<ObjectWrapper::Ptr>& nodes )
	{
		nodes.clear();
		for(auto it : m_nodes)
			nodes.push_back(it.first);
		//nodes = std::vector<ObjectWrapper::Ptr>( m_nodes.begin(), m_nodes.end() );
	}

	UpdateGraphWrapper::Node& UpdateGraphWrapper::addNode(ObjectWrapper::Ptr objectWrapper)
	{
		auto it = m_nodes.find(objectWrapper);
		if( it==m_nodes.end() )
		{
			m_nodes[objectWrapper] = Node();
			connect( objectWrapper.get(), SIGNAL(propertyAdded(const std::string&)), this, SLOT(onObjectPropertyAdded(std::string)) );
			connect( objectWrapper.get(), SIGNAL(propertyRemoved(const std::string&)), this, SLOT(onObjectPropertyRemoved(std::string)) );
			connect( objectWrapper.get(), SIGNAL(propertyReferenceChanged()), this, SLOT(onObjectPropertyReferenceChanged()) );
		}
		return m_nodes[objectWrapper];
	}

	void UpdateGraphWrapper::addConnection(ObjectWrapper::Ptr controllerWrapper, ObjectWrapper::Ptr objectWrapper, const std::string &propName)
	{
		Controller::Ptr controller = std::dynamic_pointer_cast<Controller>( controllerWrapper->getObject() );
		m_graph->addConnection( controller, objectWrapper->getObject(), propName );
		m_graph->compile();
	}

	void UpdateGraphWrapper::removeConnection(ObjectWrapper::Ptr controllerWrapper, ObjectWrapper::Ptr objectWrapper, const std::string &propName)
	{
		Controller::Ptr controller = std::dynamic_pointer_cast<Controller>( controllerWrapper->getObject() );
		m_graph->removeConnection( controller, objectWrapper->getObject(), propName );
		m_graph->compile();
	}

	void UpdateGraphWrapper::serialize(Serializer &out, houdini::json::ObjectPtr json )
	{
		houdini::json::ArrayPtr jsonNodes = houdini::json::Array::create();
		for( auto it:m_nodes )
		{
			ObjectWrapper::Ptr objectWrapper = it.first;
			Node& node = it.second;

			houdini::json::ObjectPtr jsonNode = houdini::json::Object::create();
			jsonNode->append( "object", out.serialize(objectWrapper->getObject()) );
			jsonNode->appendValue<float>( "posX", node.pos.x() );
			jsonNode->appendValue<float>( "posY", node.pos.y() );
			jsonNodes->append( jsonNode );
		}
		json->append( "nodes", jsonNodes );
	}

	void UpdateGraphWrapper::deserialize(Deserializer &in, houdini::json::ObjectPtr json)
	{
		houdini::json::ArrayPtr jsonNodes = json->getArray("nodes");
		for( int i=0, numElements=jsonNodes->size();i<numElements;++i )
		{
			houdini::json::ObjectPtr jsonNode = jsonNodes->getObject(i);

			Object::Ptr object = in.deserializeObject(jsonNode->getValue("object"));
			ObjectWrapper::Ptr objectWrapper = Application::getInstance()->getWrapper(object);
			Node& node = addNode( objectWrapper );
			node.pos.rx() = jsonNode->get<float>("posX");
			node.pos.ry() = jsonNode->get<float>("posY");
		}
	}

	void UpdateGraphWrapper::setPosition(ObjectWrapper::Ptr object, const QPointF &pos)
	{
		auto it = m_nodes.find(object);
		if(it!=m_nodes.end())
		{
			it->second.pos.rx() = pos.x();
			it->second.pos.ry() = pos.y();
		}
	}

	QPointF UpdateGraphWrapper::getPosition(ObjectWrapper::Ptr object)
	{
		auto it = m_nodes.find(object);
		if(it!=m_nodes.end())
			return it->second.pos;
		return QPointF(0,0);
	}

	void UpdateGraphWrapper::onObjectPropertyAdded(const std::string &name)
	{
		ObjectWrapper* wrapper = dynamic_cast<ObjectWrapper*>( sender() );
		std::cout << "test: " << wrapper->getName() << std::endl;
		ObjectWrapper::Ptr objectWrapper;
		for( auto it:m_nodes )
			if( wrapper == it.first.get() )
			{
				objectWrapper = it.first;
				break;
			}
		if( objectWrapper )
			emit propertyAdded( objectWrapper, name );
	}

	void UpdateGraphWrapper::onObjectPropertyRemoved(const std::string &name)
	{
		ObjectWrapper* wrapper = dynamic_cast<ObjectWrapper*>( sender() );
		ObjectWrapper::Ptr objectWrapper;
		for( auto it:m_nodes )
			if( wrapper == it.first.get() )
			{
				objectWrapper = it.first;
				break;
			}
		if( objectWrapper )
			emit propertyRemoved( objectWrapper, name );
	}

	void UpdateGraphWrapper::onObjectPropertyReferenceChanged()
	{
		std::cout << "UpdateGraphWrapper::onObjectPropertyReferenceChanged\n";
		ObjectWrapper* wrapper = dynamic_cast<ObjectWrapper*>( sender() );
		ObjectWrapper::Ptr objectWrapper;
		for( auto it:m_nodes )
			if( wrapper == it.first.get() )
			{
				objectWrapper = it.first;
				break;
			}
		if( objectWrapper )
		{
			// recompile the graph
			//TODO: recompile only for given object
			std::cout << "recompiling graph\n";
			this->m_graph->compile();
		}
	}


} // namespace gui
