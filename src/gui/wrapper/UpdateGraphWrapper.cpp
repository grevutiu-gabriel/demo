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
			m_nodes.insert(Application::getInstance()->getWrapper(node));
	}

	void UpdateGraphWrapper::getNodes( std::vector<ObjectWrapper::Ptr>& nodes )
	{
		nodes = std::vector<ObjectWrapper::Ptr>( m_nodes.begin(), m_nodes.end() );
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

} // namespace gui
