#pragma once

#include <QObject>

#include "../framework/Composition.h"
#include "CompositionElementWrapper.h"
#include "UpdateGraphWrapper.h"

namespace gui
{

	class CompositionWrapper : public ObjectWrapper
	{
		Q_OBJECT
	public:
		typedef std::shared_ptr<CompositionWrapper> Ptr;

		CompositionWrapper( Composition::Ptr composition );
		static Ptr create();
		static Ptr create(Composition::Ptr composition);

		void load();

		Composition::Ptr getComposition();
		std::string getName()const;
		UpdateGraphWrapper::Ptr getUpdateGraph();

		CompositionElementWrapper::Ptr getCompositionElement( int index );
		CompositionElementWrapper::Ptr takeCompositionElement( int index );
		void addElement( ElementWrapper::Ptr elementWrapper );

	signals:
		void compositionElementAdded( int indexAdded );
		void compositionElementRemoved( int indexRemoved );
	public slots:
	public:
		Composition::Ptr m_composition;
		std::vector<CompositionElementWrapper::Ptr> m_compositionElementWrapper;
	};

} // namespace gui
