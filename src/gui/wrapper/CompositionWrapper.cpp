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

	CompositionWrapper::Ptr CompositionWrapper::create(Composition::Ptr composition)
	{
		return std::make_shared<CompositionWrapper>(composition);
	}

	void CompositionWrapper::load()
	{
//		// for compositionelements
//		std::vector<CompositionElement::Ptr>& compositionElements = m_composition->getChilds();
//		int index = 0;
//		for( auto compositionElement:compositionElements )
//		{
//			CompositionElementWrapper::Ptr compositionElementWrapper = CompositionElementWrapper::create( compositionElement );
//			m_compositionElementWrapper.push_back(compositionElementWrapper);
//			emit compositionElementAdded(index);
//			compositionElementWrapper->load();
//			++index;
//		}

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





} // namespace gui
