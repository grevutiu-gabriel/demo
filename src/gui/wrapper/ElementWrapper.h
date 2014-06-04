#pragma once

#include "ObjectWrapper.h"

#include "../framework/Element.h"


namespace gui
{

	class ElementWrapper : public ObjectWrapper
	{
		Q_OBJECT
	public:
		typedef std::shared_ptr<ElementWrapper> Ptr;

		ElementWrapper( Element::Ptr element );

		static Ptr create( Element::Ptr element );

		Element::Ptr getElement();

	signals:
	public slots:
	public:
		Element::Ptr m_element;
	};

} // namespace gui
