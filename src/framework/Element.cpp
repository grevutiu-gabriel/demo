#include "Element.h"


/*
struct ElementMetaObject : public MetaObject
{
	ElementMetaObject() : MetaObject()
	{
		ObjectFactory::registerClass(this);
	}

	virtual Object::Ptr create()const override
	{
		return std::make_shared<Element>();
	}
	virtual const std::string& getTypeName()const override
	{
		return "Element";
	}
};

ElementMetaObject g_elementMetaObject;

*/



RenderFunction Element::evaluate(float time)
{
	return std::bind( &Element::render, this, std::placeholders::_1, std::placeholders::_2 );
}

REGISTERCLASS2( Element, Controller )
