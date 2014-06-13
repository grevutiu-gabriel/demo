#pragma once

#include <QObject>

#include "../framework/Shot.h"
#include "ShotElementWrapper.h"
#include "UpdateGraphWrapper.h"

namespace gui
{

	class ShotWrapper : public ObjectWrapper
	{
		Q_OBJECT
	public:
		typedef std::shared_ptr<ShotWrapper> Ptr;

		ShotWrapper( Shot::Ptr shot );
		static Ptr create();

		void load();

		Shot::Ptr getShot();
		std::string getName()const;
		UpdateGraphWrapper::Ptr getUpdateGraph();

		ShotElementWrapper::Ptr getShotElement( int index );
		void addElement( ElementWrapper::Ptr elementWrapper );

	signals:
		void shotElementAdded( int indexAdded );
		void shotElementRemoved( int indexRemoved );
	public slots:
	public:
		Shot::Ptr m_shot;
		std::vector<ShotElementWrapper::Ptr> m_shotElementWrapper;
	};

} // namespace gui
