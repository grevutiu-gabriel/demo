#include "SceneWrapper.h"

#include "../Application.h"
namespace gui
{

	SceneWrapper::SceneWrapper( Scene::Ptr scene ):m_scene(scene)
	{
		std::cout << "adding scene1\n";
		Application::getInstance()->watchFile(base::expand(m_scene->getFilename()), std::bind( &SceneWrapper::reload, this ));
	}

	void SceneWrapper::reload()
	{
		m_scene->reload();
		Application::getInstance()->getGlViewer()->update();
	}

} // namespace gui
