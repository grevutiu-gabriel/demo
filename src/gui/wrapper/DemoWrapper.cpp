#include "DemoWrapper.h"

#include <util/StringManip.h>

#include "ShotElementWrapper.h"

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

			//TODO: (*it)->load();
		}

		// for shots ----
		std::vector<Shot::Ptr>& shots = m_demo->getShots();
		index = 0;
		for( auto it = shots.begin(), end=shots.end();it!=end;++it,++index )
		{
			ShotWrapper::Ptr shotWrapper = std::make_shared<ShotWrapper>(*it);
			m_shotWrapper.push_back( shotWrapper );
			emit shotAdded(index);
			shotWrapper->load();
		}
	}

	void DemoWrapper::save(const std::string &filename)
	{
		m_demo->save(filename);
	}

	Demo::Ptr DemoWrapper::getDemo()
	{
		return m_demo;
	}

	void DemoWrapper::addShot(ShotWrapper::Ptr shotWrapper)
	{
		m_demo->addShot( shotWrapper->getShot() );
		int index = int(m_shotWrapper.size());
		m_shotWrapper.push_back(shotWrapper);
		emit shotAdded(index);
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

	ShotWrapper::Ptr DemoWrapper::getShotWrapper(int index)
	{
		return m_shotWrapper[index];
	}

	SceneWrapper::Ptr DemoWrapper::getSceneWrapper(int index)
	{
		return m_sceneWrapper[index];
	}

} // namespace gui
