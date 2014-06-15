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

	ShotElement()
	{
	}

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

	std::vector<ShotElement::Ptr>& getChilds()
	{
		return m_childs;
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

	Element::Ptr getElement()
	{
		return m_element;
	}

	houdini::json::Value serialize(Serializer &out);
	void deserialize(Deserializer &in, houdini::json::Value value);


	Element::Ptr m_element;
	std::vector<ShotElement::Ptr> m_childs;
};





class Shot : public Object
{
	OBJECT
public:
	typedef std::shared_ptr<Shot> Ptr;


	Shot();

	static Ptr create()
	{
		return std::make_shared<Shot>();
	}

	ShotElement::Ptr addElement( Element::Ptr element )
	{
		ShotElement::Ptr shotElement = ShotElement::create(element);
		m_elements.push_back(shotElement);
		return shotElement;
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

	void setPropertyController(Object::Ptr object, const std::string& name, Controller::Ptr controller);
	UpdateGraph::Ptr getUpdateGraph();

	base::Camera::Ptr getCamera()const
	{
		return m_camera;
	}
	void setCamera( base::Camera::Ptr camera )
	{
		m_camera = camera;
	}

	int getNumShotElements()const;
	ShotElement::Ptr getShotElement( int index );
	std::vector<ShotElement::Ptr>& getShotElements();


	virtual void serialize(Serializer &out);
	virtual void deserialize(Deserializer &in);

	base::Camera::Ptr                        m_camera;
	std::vector<ShotElement::Ptr>            m_elements;

	UpdateGraph::Ptr                         m_updateGraph;
};


