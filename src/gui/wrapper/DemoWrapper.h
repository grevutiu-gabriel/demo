#pragma once

#include <QObject>

#include "../framework/Demo.h"
#include "SceneWrapper.h"
#include "CompositionWrapper.h"


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
		void save(const std::string& filename);
		Demo::Ptr getDemo();

		void addComposition( CompositionWrapper::Ptr compositionWrapper );
		void loadScene( const std::string& filename );

		CompositionWrapper::Ptr getCompositionWrapper(int index);
		SceneWrapper::Ptr getSceneWrapper( int index );


	signals:
		void sceneAdded( int index );
		void compositionAdded( int index );
	public slots:
	private:
		Demo::Ptr m_demo;
		std::vector<SceneWrapper::Ptr> m_sceneWrapper;
		std::vector<CompositionWrapper::Ptr> m_compositionWrapper;
	};

} // namespace gui
