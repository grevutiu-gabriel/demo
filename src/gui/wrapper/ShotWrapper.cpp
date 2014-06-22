#include "ShotWrapper.h"

#include "../Application.h"
namespace gui
{

	ShotWrapper::ShotWrapper( Shot::Ptr shot ):
		ObjectWrapper(shot),
		m_shot(shot)
	{
	}

	ShotWrapper::Ptr ShotWrapper::create()
	{
		Shot::Ptr shot = Shot::create();
		shot->setName("new shot");
		return std::make_shared<ShotWrapper>(shot);
	}

	void ShotWrapper::load()
	{
		// for shotelements
		std::vector<ShotElement::Ptr>& shotElements = m_shot->getShotElements();
		int index = 0;
		for( auto shotElement:shotElements )
		{
			ShotElementWrapper::Ptr shotElementWrapper = ShotElementWrapper::create( shotElement );
			m_shotElementWrapper.push_back(shotElementWrapper);
			emit shotElementAdded(index);
			shotElementWrapper->load();
			++index;
		}

		Application::getInstance()->getWrapper(getShot()->getUpdateGraph())->load();
	}

	Shot::Ptr ShotWrapper::getShot()
	{
		return m_shot;
	}


	std::string ShotWrapper::getName() const
	{
		return m_shot->getName();
	}

	UpdateGraphWrapper::Ptr ShotWrapper::getUpdateGraph()
	{
		return Application::getInstance()->getWrapper(m_shot->getUpdateGraph());
	}

	ShotElementWrapper::Ptr ShotWrapper::getShotElement(int index)
	{
		return m_shotElementWrapper[index];
	}

	void ShotWrapper::addElement(ElementWrapper::Ptr elementWrapper)
	{
		ShotElement::Ptr shotElement = m_shot->addElement(elementWrapper->getElement());
		int index = int(m_shotElementWrapper.size());
		m_shotElementWrapper.push_back(ShotElementWrapper::create(shotElement));
		emit shotElementAdded(index);
		Application::getInstance()->getGlViewer()->update();
	}

	ShotElementWrapper::Ptr ShotWrapper::takeShotElement(int index)
	{
		m_shot->takeShotElement(index);

		// reflect this in the wrapper list
		//TODO: use Application to get wrapper instead of maintaining own list
		ShotElementWrapper::Ptr sew = getShotElement(index);
		m_shotElementWrapper.erase(m_shotElementWrapper.begin()+index);

		emit shotElementRemoved(index);

		Application::getInstance()->getGlViewer()->update();

		return sew;
	}

} // namespace gui
