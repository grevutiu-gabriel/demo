#pragma once

#include <QObject>

#include "../framework/Scene.h"


namespace gui
{

	class SceneWrapper : public QObject
	{
		Q_OBJECT
	public:
		typedef std::shared_ptr<SceneWrapper> Ptr;

		SceneWrapper( Scene::Ptr scene );
		void reload();
		void getControllerNames( std::vector<std::string>& names );
		std::string getName()const;

	signals:
		void sceneReloaded();
	public slots:
	public:
		Scene::Ptr m_scene;
	};

} // namespace gui
