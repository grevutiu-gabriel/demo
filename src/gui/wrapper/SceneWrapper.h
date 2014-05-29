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

	signals:
	public slots:
	public:
		Scene::Ptr m_scene;
	};

} // namespace gui
