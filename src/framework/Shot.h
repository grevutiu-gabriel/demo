#pragma once

#include <util/shared_ptr.h>
#include <gfx/Context.h>
#include <gfx/Camera.h>

#include "Scene.h"
#include "Element.h"

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


	Element::Ptr m_element;
	std::vector<ShotElement::Ptr> m_childs;
};



struct Shot
{
	typedef std::shared_ptr<Shot> Ptr;


	Shot()
	{
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



	virtual void render( base::Context::Ptr context, float time, base::Camera::Ptr overrideCamera )
	{
		// update animated properties
		for(auto it=m_constantProperties.begin(),end=m_constantProperties.end();it!=end;++it)
			it->second->update( it->first, time);
		for(auto it=m_animatedProperties.begin(),end=m_animatedProperties.end();it!=end;++it)
			it->second->update( it->first, time);

		base::Camera::Ptr camera;
		if( overrideCamera )
			camera = overrideCamera;
		else
		if( m_cameraController )
			camera = m_cameraController->evaluate(time);

		context->setView( camera->m_worldToView, camera->m_viewToWorld, camera->m_viewToNDC );

		// render elements
		for( auto it = m_elements.begin(), end=m_elements.end();it!=end;++it )
		{
			ShotElement::Ptr shotElement = *it;
			shotElement->render(context, time);
		}
	}

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

	CameraController::Ptr                    m_cameraController;
	std::vector<ShotElement::Ptr>            m_elements;
	std::map<Property::Ptr, Controller::Ptr> m_constantProperties;
	std::map<Property::Ptr, Controller::Ptr> m_animatedProperties;
};


