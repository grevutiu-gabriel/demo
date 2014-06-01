#pragma once

#include <QObject>

#include "../framework/Demo.h"
#include "SceneWrapper.h"
#include "ShotWrapper.h"


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

		void addShot( ShotWrapper::Ptr shotWrapper );
		void loadScene( const std::string& filename );

		ShotWrapper::Ptr getShotWrapper(int index);
		SceneWrapper::Ptr getSceneWrapper( int index );


	signals:
		void sceneAdded( int index );
		void shotAdded( int index );
	public slots:
	private:
		Demo::Ptr m_demo;
		std::vector<SceneWrapper::Ptr> m_sceneWrapper;
		std::vector<ShotWrapper::Ptr> m_shotWrapper;
	};

} // namespace gui
