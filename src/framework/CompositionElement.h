#pragma once

#include <util/shared_ptr.h>
#include <gfx/Context.h>
#include <gfx/Camera.h>

#include "Scene.h"
#include "Element.h"
#include "Controller.h"
#include "UpdateGraph.h"

#include <iostream>



class CompositionElement : public Object
{
	OBJECT
public:
	typedef std::shared_ptr<CompositionElement> Ptr;

	CompositionElement();
	CompositionElement(Element::Ptr element);

	static Ptr create( Element::Ptr element );

	Ptr                                   addChild( Element::Ptr element );
	std::vector<CompositionElement::Ptr>& getChilds();
	CompositionElement::Ptr               getChild( int index );
	CompositionElement::Ptr               takeChild(int index);
	// propagates the given vector with own element and all child elements
	// used by composition::prepareForRendering to find the root nodes of the updategraph
	void                                  getAllChildElements( std::vector<Element::Ptr>& elements );
	void                                  render(base::Context::Ptr context, float time);

	Element::Ptr                          getElement();

	void                                  serialize(Serializer &out);
	void                                  deserialize(Deserializer &in);

protected:
	Element::Ptr                         m_element;
	std::vector<CompositionElement::Ptr> m_childs;
};




