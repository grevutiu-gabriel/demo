#include "SceneWrapper.h"

#include "../Application.h"
namespace gui
{

	SceneWrapper::SceneWrapper( Scene::Ptr scene ):
		ObjectWrapper(scene),
		m_scene(scene)
	{
		Application::getInstance()->watchFile(base::expand(m_scene->getFilename()), std::bind( &SceneWrapper::reload, this ));
	}

	void SceneWrapper::reload()
	{
		m_scene->reload();
		Application::getInstance()->getGlViewer()->update();
		emit sceneReloaded();
	}

	void SceneWrapper::getControllerNames(std::vector<std::string> &names)
	{
		m_scene->getControllerNames( names );
	}

	Scene::Ptr SceneWrapper::getScene()
	{
		return m_scene;
	}

	std::string SceneWrapper::getName() const
	{
		return m_scene->getName();
	}

} // namespace gui
