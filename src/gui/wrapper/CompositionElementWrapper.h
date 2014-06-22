#pragma once

#include <QObject>

#include "../framework/Composition.h"
#include "ElementWrapper.h"

namespace gui
{

	class CompositionElementWrapper : public QObject
	{
		Q_OBJECT
	public:
		typedef std::shared_ptr<CompositionElementWrapper> Ptr;

		CompositionElementWrapper( CompositionElement::Ptr compositionElement );

		static Ptr create( CompositionElement::Ptr compositionElement );
		void load();

		CompositionElement::Ptr getCompositionElement();
		ElementWrapper::Ptr getElement();

	signals:
		void compositionElementAdded( int indexAdded );
		void compositionElementRemoved( int indexRemoved );
	public slots:
	public:
		CompositionElement::Ptr                     m_compositionElement;
		std::vector<CompositionElementWrapper::Ptr> m_childs;
		ElementWrapper::Ptr                  m_elementWrapper;
	};

} // namespace gui
