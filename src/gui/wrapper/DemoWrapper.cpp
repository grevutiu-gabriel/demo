#include "DemoWrapper.h"

namespace gui
{

	DemoWrapper::DemoWrapper( Demo::Ptr demo ):m_demo(demo)
	{
		// inspect demo and create wrappers
		std::vector<Scene::Ptr>& scenes = m_demo->getScenes();
		for( auto it = scenes.begin(), end=scenes.end();it!=end;++it )
		{
			m_sceneWrapper.push_back( std::make_shared<SceneWrapper>(*it) );
		}
	}

} // namespace gui
