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
/*
	ShotWrapper::Ptr ShotWrapper::create()
	{
		Shot::Ptr shot = Shot::create();
		shot->setName("new shot");
		return std::make_shared<ShotWrapper>(shot);
	}

	void ShotWrapper::load()
	{
		// for shotelements
		std::vector<ShotElement::Ptr>& shotElements = m_shot->getShotElements();
		int index = 0;
		for( auto shotElement:shotElements )
		{
			ShotElementWrapper::Ptr shotElementWrapper = ShotElementWrapper::create( shotElement );
			m_shotElementWrapper.push_back(shotElementWrapper);
			emit shotElementAdded(index);
			shotElementWrapper->load();
			++index;
		}
	}

	Shot::Ptr ShotWrapper::getShot()
	{
		return m_shot;
	}


	std::string ShotWrapper::getName() const
	{
		return m_shot->getName();
	}

	ShotElementWrapper::Ptr ShotWrapper::getShotElement(int index)
	{
		return m_shotElementWrapper[index];
	}

	void ShotWrapper::addShotElement(ShotElementWrapper::Ptr shotElementWrapper)
	{
		int index = int(m_shotElementWrapper.size());
		m_shotElementWrapper.push_back(shotElementWrapper);
		emit shotElementAdded(index);
	}
*/
} // namespace gui
