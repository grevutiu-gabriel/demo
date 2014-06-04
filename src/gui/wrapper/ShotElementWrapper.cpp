#include "ShotElementWrapper.h"

#include "../Application.h"
namespace gui
{

	ShotElementWrapper::ShotElementWrapper( ShotElement::Ptr shotElement ):m_shotElement(shotElement)
	{
		m_elementWrapper = std::dynamic_pointer_cast<ElementWrapper>(Application::getInstance()->getWrapper( shotElement->getElement() ));
	}

	ShotElementWrapper::Ptr ShotElementWrapper::create( ShotElement::Ptr shotElement )
	{
		return std::make_shared<ShotElementWrapper>(shotElement);
	}

	void ShotElementWrapper::load()
	{
		// for shotelements
		std::vector<ShotElement::Ptr>& shotElements = m_shotElement->getChilds();
		int index = 0;
		for( auto shotElement:shotElements )
		{
			ShotElementWrapper::Ptr shotElementWrapper = ShotElementWrapper::create( shotElement );
			m_childs.push_back(shotElementWrapper);
			emit shotElementAdded(index);
			shotElementWrapper->load();
			++index;
		}

	}

	ShotElement::Ptr ShotElementWrapper::getShotElement()
	{
		return m_shotElement;
	}

	ElementWrapper::Ptr ShotElementWrapper::getElement()
	{
		return m_elementWrapper;
	}



} // namespace gui
