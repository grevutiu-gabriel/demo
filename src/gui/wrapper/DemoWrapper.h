#pragma once

#include <QObject>

#include "../framework/Demo.h"
#include "SceneWrapper.h"


namespace gui
{

	class DemoWrapper : public QObject
	{
		Q_OBJECT
	public:
		typedef std::shared_ptr<DemoWrapper> Ptr;

		DemoWrapper( Demo::Ptr demo );


	signals:
	public slots:
	private:
		Demo::Ptr m_demo;
		std::vector<SceneWrapper::Ptr> m_sceneWrapper;
	};

} // namespace gui
