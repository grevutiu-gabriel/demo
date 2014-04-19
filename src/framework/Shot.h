#pragma once

#include <util/shared_ptr.h>
#include <gfx/Context.h>
#include <gfx/Camera.h>

#include "Scene.h"
#include "Element.h"

#include <iostream>







struct Shot
{
	typedef std::shared_ptr<Shot> Ptr;

	struct ShotElement
	{
		typedef std::shared_ptr<ShotElement> Ptr;

		ShotElement( Element::Ptr element ) : m_element(element)
		{
		}

		static Ptr create( Element::Ptr element )
		{
			return std::make_shared<ShotElement>( element );
		}

		Ptr addChild( Element::Ptr element )
		{
			Ptr child = create(element);
			m_childs.push_back( child );
			return child;
		}

		void render( base::Context::Ptr context, float time )
		{
			// update animated properties
			for(auto it=m_constantProperties.begin(),end=m_constantProperties.end();it!=end;++it)
				it->second->update( it->first, time);
			for(auto it=m_animatedProperties.begin(),end=m_animatedProperties.end();it!=end;++it)
				it->second->update( it->first, time);

			// render childs if there are any
			if( !m_childs.empty() )
			{
				m_element->begin(context);
				for( auto it=m_childs.begin(),end=m_childs.end();it!=end;++it )
					(*it)->render(context, time);
				m_element->end(context);
			}
			// finally render the element
			m_element->render(context);
		}

		void setController( const std::string& name, Controller::Ptr controller )
		{
			Property::Ptr prop = m_element->getProperty( name );
			if(prop)
			{
				if(controller->isAnimated())
					m_animatedProperties[prop] = controller;
				else
					m_constantProperties[prop] = controller;
			}
		}

		Element::Ptr m_element;
		std::map<Property::Ptr, Controller::Ptr> m_constantProperties;
		std::map<Property::Ptr, Controller::Ptr> m_animatedProperties;
		std::vector<Ptr> m_childs;
	};

	Shot( Camera::Ptr camera = Camera::Ptr() ) : m_camera(camera)
	{
	}

	static Ptr create( Camera::Ptr camera )
	{
		return std::make_shared<Shot>( camera );
	}

	ShotElement::Ptr addElement( Element::Ptr element )
	{
		ShotElement::Ptr se = ShotElement::create(element);
		m_elements.push_back(se);
		return se;
	}
	void addElement( ShotElement::Ptr se )
	{
		m_elements.push_back(se);
	}

	virtual void render( base::Context::Ptr context, float time, base::Camera::Ptr overrideCamera )
	{
		if( overrideCamera )
		{
			context->setView( overrideCamera->m_worldToView, overrideCamera->m_viewToWorld, overrideCamera->m_viewToNDC );
		}else
			// set view from our own camera
			context->setView( m_camera->xform->evaluate(time), m_camera->projectionMatrix->evaluate(time)  );

		// render elements
		for( auto it = m_elements.begin(), end=m_elements.end();it!=end;++it )
		{
			ShotElement::Ptr se = *it;
			se->render(context, time);
		}
	}


	Camera::Ptr                   m_camera;
	std::vector<ShotElement::Ptr> m_elements;
};

// camera is driven by a switcher
// convinience for having the same element setup shot from many different cameras
struct SwitchedShot : public Shot
{
	typedef std::shared_ptr<SwitchedShot> Ptr;
	SwitchedShot( Switcher::Ptr switcher ) :
		Shot(),
		m_switcher(switcher)
	{
	}
	static Ptr create( Switcher::Ptr switcher )
	{
		return std::make_shared<SwitchedShot>( switcher );
	}
	virtual void render( base::Context::Ptr context, float time, base::Camera::Ptr overrideCamera )override
	{
		int sw = int(m_switcher->m_switch->evaluate(time));
		m_camera = m_switcher->m_cameras[ sw ];
		Shot::render(context, time, overrideCamera);
	}

	Switcher::Ptr m_switcher;

};
