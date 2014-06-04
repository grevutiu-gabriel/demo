#pragma once

#include <QObject>

#include "../framework/Shot.h"
#include "ElementWrapper.h"

namespace gui
{

	class ShotElementWrapper : public QObject
	{
		Q_OBJECT
	public:
		typedef std::shared_ptr<ShotElementWrapper> Ptr;

		ShotElementWrapper( ShotElement::Ptr shotElement );

		static Ptr create( ShotElement::Ptr shotElement );
		void load();

		ShotElement::Ptr getShotElement();
		ElementWrapper::Ptr getElement();

	signals:
		void shotElementAdded( int indexAdded );
		void shotElementRemoved( int indexRemoved );
	public slots:
	public:
		ShotElement::Ptr                     m_shotElement;
		std::vector<ShotElementWrapper::Ptr> m_childs;
		ElementWrapper::Ptr                  m_elementWrapper;
	};

} // namespace gui
