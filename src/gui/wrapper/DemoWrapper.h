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

		DemoWrapper();
		static Ptr create();

		void load(const std::string& filename);
		Demo::Ptr getDemo();

		void loadScene( const std::string& filename );

		SceneWrapper::Ptr getSceneWrapper( int index );


	signals:
		void sceneAdded( int index );
	public slots:
	private:
		Demo::Ptr m_demo;
		std::vector<SceneWrapper::Ptr> m_sceneWrapper;
	};

} // namespace gui
