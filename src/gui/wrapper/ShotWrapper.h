#pragma once

#include <QObject>

#include "../framework/Shot.h"


namespace gui
{

	class ShotWrapper : public QObject
	{
		Q_OBJECT
	public:
		typedef std::shared_ptr<ShotWrapper> Ptr;

		ShotWrapper( Shot::Ptr shot );

		static Ptr create();

		Shot::Ptr getShot();
		std::string getName()const;

	signals:
	public slots:
	public:
		Shot::Ptr m_shot;
	};

} // namespace gui
