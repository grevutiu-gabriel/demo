#pragma once

#include <QObject>

#include "../framework/Composition.h"
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


	signals:
	public slots:
	public:
		Composition::Ptr m_composition;
	};

} // namespace gui
