#include "CompositionElementWrapper.h"

#include "../Application.h"
namespace gui
{

	CompositionElementWrapper::CompositionElementWrapper( CompositionElement::Ptr compositionElement ):
		ObjectWrapper(compositionElement),
		m_compositionElement(compositionElement)
	{
		m_elementWrapper = std::dynamic_pointer_cast<ElementWrapper>(Application::getInstance()->getWrapper( compositionElement->getElement() ));
	}

	CompositionElementWrapper::Ptr CompositionElementWrapper::create( CompositionElement::Ptr compositionElement )
	{
		return std::make_shared<CompositionElementWrapper>(compositionElement);
	}

	void CompositionElementWrapper::load()
	{
		// for compositionelements
		std::vector<CompositionElement::Ptr>& compositionElements = m_compositionElement->getChilds();
		int index = 0;
		for( auto compositionElement:compositionElements )
		{
			CompositionElementWrapper::Ptr compositionElementWrapper = CompositionElementWrapper::create( compositionElement );
			m_childs.push_back(compositionElementWrapper);
			emit compositionElementAdded(index);
			compositionElementWrapper->load();
			++index;
		}

	}

	CompositionElement::Ptr CompositionElementWrapper::getCompositionElement()
	{
		return m_compositionElement;
	}

	ElementWrapper::Ptr CompositionElementWrapper::getElement()
	{
		return m_elementWrapper;
	}



} // namespace gui
