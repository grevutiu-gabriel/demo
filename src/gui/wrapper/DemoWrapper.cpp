#include "DemoWrapper.h"

#include <util/StringManip.h>

namespace gui
{

	DemoWrapper::DemoWrapper():
		QObject(),
		m_demo(Demo::create())
	{
	}

	DemoWrapper::Ptr DemoWrapper::create()
	{
		return std::make_shared<DemoWrapper>();
	}

	void DemoWrapper::load(const std::string &filename)
	{
		m_demo->load(filename);

		// inspect demo and create wrappers

		// for scenes --------
		std::vector<Scene::Ptr>& scenes = m_demo->getScenes();
		int index = 0;
		for( auto it = scenes.begin(), end=scenes.end();it!=end;++it,++index )
		{
			SceneWrapper::Ptr sceneWrapper = std::make_shared<SceneWrapper>(*it);
			m_sceneWrapper.push_back( sceneWrapper );
			emit sceneAdded(index);
		}
	}

	Demo::Ptr DemoWrapper::getDemo()
	{
		return m_demo;
	}

	void DemoWrapper::loadScene(const std::string &filename)
	{
		std::string name = filename;
		Scene::Ptr scene = Scene::create();
		std::cout << "loading " << filename << std::endl;
		scene->load(filename);
		scene->setName(base::replace(name, base::path("data").str(), "$DATA"));
		SceneWrapper::Ptr sceneWrapper = std::make_shared<SceneWrapper>(scene);
		int index = int(m_sceneWrapper.size());
		m_sceneWrapper.push_back(sceneWrapper);
		emit sceneAdded(index);
	}

	SceneWrapper::Ptr DemoWrapper::getSceneWrapper(int index)
	{
		return m_sceneWrapper[index];
	}

} // namespace gui
