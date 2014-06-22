#include "CompositionWrapper.h"

#include "../Application.h"
namespace gui
{

	CompositionWrapper::CompositionWrapper( Composition::Ptr composition ):
		ObjectWrapper(composition),
		m_composition(composition)
	{
	}

	CompositionWrapper::Ptr CompositionWrapper::create()
	{
		Composition::Ptr composition = Composition::create();
		composition->setName("new composition");
		return std::make_shared<CompositionWrapper>(composition);
	}

	void CompositionWrapper::load()
	{
		// for compositionelements
		std::vector<CompositionElement::Ptr>& compositionElements = m_composition->getCompositionElements();
		int index = 0;
		for( auto compositionElement:compositionElements )
		{
			CompositionElementWrapper::Ptr compositionElementWrapper = CompositionElementWrapper::create( compositionElement );
			m_compositionElementWrapper.push_back(compositionElementWrapper);
			emit compositionElementAdded(index);
			compositionElementWrapper->load();
			++index;
		}

		Application::getInstance()->getWrapper(getComposition()->getUpdateGraph())->load();
	}

	Composition::Ptr CompositionWrapper::getComposition()
	{
		return m_composition;
	}


	std::string CompositionWrapper::getName() const
	{
		return m_composition->getName();
	}

	UpdateGraphWrapper::Ptr CompositionWrapper::getUpdateGraph()
	{
		return Application::getInstance()->getWrapper(m_composition->getUpdateGraph());
	}

	CompositionElementWrapper::Ptr CompositionWrapper::getCompositionElement(int index)
	{
		return m_compositionElementWrapper[index];
	}

	void CompositionWrapper::addElement(ElementWrapper::Ptr elementWrapper)
	{
		CompositionElement::Ptr compositionElement = m_composition->addElement(elementWrapper->getElement());
		int index = int(m_compositionElementWrapper.size());
		m_compositionElementWrapper.push_back(CompositionElementWrapper::create(compositionElement));
		emit compositionElementAdded(index);
		Application::getInstance()->getGlViewer()->update();
	}

	CompositionElementWrapper::Ptr CompositionWrapper::takeCompositionElement(int index)
	{
		m_composition->takeCompositionElement(index);

		// reflect this in the wrapper list
		//TODO: use Application to get wrapper instead of maintaining own list
		CompositionElementWrapper::Ptr sew = getCompositionElement(index);
		m_compositionElementWrapper.erase(m_compositionElementWrapper.begin()+index);

		emit compositionElementRemoved(index);

		Application::getInstance()->getGlViewer()->update();

		return sew;
	}

} // namespace gui
