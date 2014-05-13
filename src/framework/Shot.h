#pragma once

#include <util/shared_ptr.h>
#include <gfx/Context.h>
#include <gfx/Camera.h>

#include "Scene.h"
#include "Element.h"
#include "Controller.h"
#include "UpdateGraph.h"

#include <iostream>



struct ShotElement
{
	typedef std::shared_ptr<ShotElement> Ptr;

	ShotElement(Element::Ptr element):
		m_element(element)
	{

	}

	static Ptr create( Element::Ptr element )
	{
		return std::make_shared<ShotElement>(element);
	}

	Ptr addChild( Element::Ptr element )
	{
		ShotElement::Ptr se = ShotElement::create(element);
		m_childs.push_back( se );
		return se;
	}

	// propagates the given vector with own element and all child elements
	// used by shot::prepareForRendering to find the root nodes of the updategraph
	void getAllChildElements( std::vector<Element::Ptr>& elements )
	{
		elements.push_back(m_element);
		for( auto it = m_childs.begin(), end=m_childs.end();it!=end;++it )
		{
			ShotElement::Ptr child = *it;
			child->getAllChildElements(elements);
		}
	}

	void render(base::Context::Ptr context, float time)
	{
		m_element->begin( context, time );
		for( auto it = m_childs.begin(), end=m_childs.end();it!=end;++it )
		{
			ShotElement::Ptr child = *it;
			child->render(context, time);
		}
		m_element->end( context, time );
		m_element->render( context, time );
	}

	houdini::json::Value serialize(Serializer &out);


	Element::Ptr m_element;
	std::vector<ShotElement::Ptr> m_childs;
};





class Shot : public Object
{
	OBJECT
public:
	typedef std::shared_ptr<Shot> Ptr;


	Shot() : Object()
	{
		addProperty<base::Camera::Ptr>( "camera", std::bind( &Shot::getCamera, this ), std::bind( &Shot::setCamera, this, std::placeholders::_1 ) );
	}

	static Ptr create()
	{
		return std::make_shared<Shot>();
	}

	void addElement( Element::Ptr element )
	{
		m_elements.push_back(ShotElement::create(element));
	}
	void addElement( ShotElement::Ptr shotElement )
	{
		m_elements.push_back(shotElement);
	}

	Element::Ptr getElement( int index )
	{
		return m_elements[index]->m_element;
	}

	int getNumElements()const
	{
		return int(m_elements.size());
	}


	virtual void prepareForRendering();

	virtual void render( base::Context::Ptr context, float time, base::Camera::Ptr overrideCamera );

	void setPropertyController(Object::Ptr object, const std::string& name, Controller::Ptr controller)
	{
		m_updateGraph.addConnection( controller, object, name );
	}

	base::Camera::Ptr getCamera()const
	{
		return m_camera;
	}
	void setCamera( base::Camera::Ptr camera )
	{
		m_camera = camera;
	}


	virtual void serialize(Serializer &out);

	CameraController::Ptr                    m_cameraController;
	base::Camera::Ptr                        m_camera;
	std::vector<ShotElement::Ptr>            m_elements;

	UpdateGraph                              m_updateGraph;

	// deprecated
	void setController( Object::Ptr object, const std::string& name, Controller::Ptr controller )
	{
		Property::Ptr prop = object->getProperty( name );
		if(prop)
		{
			if(controller->isAnimated())
				m_animatedProperties[prop] = controller;
			else
				m_constantProperties[prop] = controller;
		}
	}

	void setController( Object::Ptr object, const std::string& name, Property::Ptr controllerProp )
	{
		RefProperty::Ptr refcontrlprop = std::dynamic_pointer_cast<RefProperty>( controllerProp );
		if( refcontrlprop)
		{
			Object::Ptr obj = refcontrlprop->getRef();
			Controller::Ptr controller = std::dynamic_pointer_cast<Controller>(obj);
			if(controller)
			{
				setController( object, name, controller );
			}
		}
	}
	std::map<Property::Ptr, Controller::Ptr> m_constantProperties;
	std::map<Property::Ptr, Controller::Ptr> m_animatedProperties;
};


