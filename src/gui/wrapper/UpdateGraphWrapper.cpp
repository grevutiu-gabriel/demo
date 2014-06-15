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

	void UpdateGraphWrapper::addNode(ObjectWrapper::Ptr objectWrapper)
	{
		auto it = m_nodes.find(objectWrapper);
		if( it==m_nodes.end() )
			m_nodes[objectWrapper] = Node();
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

} // namespace gui
