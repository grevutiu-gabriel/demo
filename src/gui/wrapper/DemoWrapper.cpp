#include "DemoWrapper.h"

#include <util/StringManip.h>

#include "../Application.h"

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
		Demo::GuiInfoDeserializationCallback deserializeGui = std::bind( &Application::deserializeGuiInfo, Application::getInstance(), std::placeholders::_1 );
		m_demo->load(filename, deserializeGui);

		// inspect demo and create wrappers

		// for scenes --------
		std::vector<Scene::Ptr>& scenes = m_demo->getScenes();
		int index = 0;
		for( auto it = scenes.begin(), end=scenes.end();it!=end;++it,++index )
		{
			SceneWrapper::Ptr sceneWrapper = std::make_shared<SceneWrapper>(*it);
			m_sceneWrapper.push_back( sceneWrapper );
			emit sceneAdded(index);

			//TODO: (*it)->load();
		}

		// for compositions ----
		std::vector<Composition::Ptr>& compositions = m_demo->getCompositions();
		index = 0;
		for( auto it = compositions.begin(), end=compositions.end();it!=end;++it,++index )
		{
			CompositionWrapper::Ptr compositionWrapper = std::make_shared<CompositionWrapper>(*it);
			m_compositionWrapper.push_back( compositionWrapper );
			emit compositionAdded(index);
			compositionWrapper->load();
		}

		Application::getInstance()->getGlViewer()->update();
	}

	void DemoWrapper::save(const std::string &filename)
	{
		Demo::GuiInfoSerializationCallback serializeGui = std::bind( &Application::serializeGuiInfo, Application::getInstance(), std::placeholders::_1 );
		m_demo->save(filename, serializeGui);
	}

	Demo::Ptr DemoWrapper::getDemo()
	{
		return m_demo;
	}

	void DemoWrapper::addComposition(CompositionWrapper::Ptr compositionWrapper)
	{
		m_demo->addComposition( compositionWrapper->getComposition() );
		int index = int(m_compositionWrapper.size());
		m_compositionWrapper.push_back(compositionWrapper);
		emit compositionAdded(index);
	}

	void DemoWrapper::loadScene(const std::string &filename)
	{
		std::string expanded = filename;
		expanded = base::replace(expanded, base::path("data").str(), "$DATA");
		std::string name = expanded;
		Scene::Ptr scene = Scene::create();
		std::cout << "loading " << filename << std::endl;
		scene->load(expanded);
		scene->setName(name);
		m_demo->addScene(scene);
		SceneWrapper::Ptr sceneWrapper = std::make_shared<SceneWrapper>(scene);
		int index = int(m_sceneWrapper.size());
		m_sceneWrapper.push_back(sceneWrapper);
		emit sceneAdded(index);
	}

	CompositionWrapper::Ptr DemoWrapper::getCompositionWrapper(int index)
	{
		return m_compositionWrapper[index];
	}

	SceneWrapper::Ptr DemoWrapper::getSceneWrapper(int index)
	{
		return m_sceneWrapper[index];
	}

} // namespace gui
