#include "ElementWrapper.h"

#include "../Application.h"
namespace gui
{

	ElementWrapper::ElementWrapper( Element::Ptr element ): ObjectWrapper(element), m_element(element)
	{
	}

	ElementWrapper::Ptr ElementWrapper::create( Element::Ptr element )
	{
		return std::make_shared<ElementWrapper>(element);
	}

	Element::Ptr ElementWrapper::getElement()
	{
		return m_element;
	}



} // namespace gui
