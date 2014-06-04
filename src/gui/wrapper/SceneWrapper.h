#pragma once

#include "ObjectWrapper.h"

#include "../framework/Scene.h"


namespace gui
{

	class SceneWrapper : public ObjectWrapper
	{
		Q_OBJECT
	public:
		typedef std::shared_ptr<SceneWrapper> Ptr;

		SceneWrapper( Scene::Ptr scene );
		void reload();
		void getControllerNames( std::vector<std::string>& names );
		Scene::Ptr getScene();
		std::string getName()const;

	signals:
		void sceneReloaded();
	public slots:
	public:
		Scene::Ptr m_scene;
	};

} // namespace gui
