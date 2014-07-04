#include "CompositionElement.h"



CompositionElement::CompositionElement():
	ControllerT<RenderFunction>()
{

}

CompositionElement::CompositionElement(Element::Ptr element):
	m_element(element)
{
}

CompositionElement::Ptr CompositionElement::create(Element::Ptr element)
{
	return std::make_shared<CompositionElement>(element);
}


RenderFunction CompositionElement::evaluate(float time)
{
	return std::bind( &CompositionElement::render, this, std::placeholders::_1, std::placeholders::_2 );
}



CompositionElement::Ptr CompositionElement::addChild(Element::Ptr element)
{
	CompositionElement::Ptr se = CompositionElement::create(element);
	m_childs.push_back( se );
	return se;
}

std::vector<CompositionElement::Ptr> &CompositionElement::getChilds()
{
	return m_childs;
}

void CompositionElement::getAllChildElements(std::vector<Element::Ptr> &elements)
{
	elements.push_back(m_element);
	for( auto it = m_childs.begin(), end=m_childs.end();it!=end;++it )
	{
		CompositionElement::Ptr child = *it;
		child->getAllChildElements(elements);
	}
}

void CompositionElement::render(base::Context::Ptr context, float time)
{
	m_element->begin( context, time );
	for( auto it = m_childs.begin(), end=m_childs.end();it!=end;++it )
	{
		CompositionElement::Ptr child = *it;
		child->render(context, time);
	}
	m_element->end( context, time );
	m_element->render( context, time );
}

Element::Ptr CompositionElement::getElement()
{
	return m_element;
}

CompositionElement::Ptr CompositionElement::getChild( int index )
{
	return m_childs[index];
}

CompositionElement::Ptr CompositionElement::takeChild(int index)
{
	CompositionElement::Ptr se = getChild(index);
	m_childs.erase(m_childs.begin()+index);
	return se;
}

void CompositionElement::serialize(Serializer &out)
{
	Object::serialize(out);


	if(m_element)
		out.write( "element", out.serialize( m_element ));

	// childs
	{
		houdini::json::ArrayPtr childs = houdini::json::Array::create();
		for( auto it=m_childs.begin(), end=m_childs.end();it!=end;++it )
		{
			CompositionElement::Ptr child = *it;
			childs->append( out.serialize(child) );
		}
		out.write("childs", childs);
	}

}

void CompositionElement::deserialize(Deserializer &in)
{
	Object::deserialize(in);

	if( in.hasKey("element") )
		m_element = std::dynamic_pointer_cast<Element>( in.deserializeObject(in.readValue("element")) );

	// childs
	{
		houdini::json::ArrayPtr childs = in.readArray("childs");
		for( int i=0,numElements=childs->size();i<numElements;++i )
		{
			CompositionElement::Ptr child = std::dynamic_pointer_cast<CompositionElement>( in.deserializeObject(childs->getValue(i)) );
			m_childs.push_back(child);
		}
	}
}




REGISTERCLASS2( CompositionElement, Controller )







