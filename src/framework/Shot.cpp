#include "Shot.h"



Shot::Shot() :
	Object(),
	m_updateGraph(std::make_shared<UpdateGraph>())
{
	addProperty<base::Camera::Ptr>( "camera", std::bind( &Shot::getCamera, this ), std::bind( &Shot::setCamera, this, std::placeholders::_1 ) );
}


void Shot::prepareForRendering()
{
	m_updateGraph->compile();
}

void Shot::render( base::Context::Ptr context, float time, base::Camera::Ptr overrideCamera )
{
	// update all properties ---
	m_updateGraph->update(time);

	// get camera ---
	base::Camera::Ptr camera = m_camera;
	if( overrideCamera )
		camera = overrideCamera;

	//if( camera )
		context->setView( camera->m_worldToView, camera->m_viewToWorld, camera->m_viewToNDC );

	// render elements
	for( auto it = m_elements.begin(), end=m_elements.end();it!=end;++it )
	{
		ShotElement::Ptr shotElement = *it;
		shotElement->render(context, time);
	}
}

void Shot::setPropertyController(Object::Ptr object, const std::string &name, Controller::Ptr controller)
{
	m_updateGraph->addConnection( controller, object, name );
}

UpdateGraph::Ptr Shot::getUpdateGraph()
{
	return m_updateGraph;
}

int Shot::getNumShotElements() const
{
	return int(m_elements.size());
}

ShotElement::Ptr Shot::getShotElement( int index )
{
	return m_elements[index];
}

std::vector<ShotElement::Ptr> &Shot::getShotElements()
{
	return m_elements;
}



void Shot::serialize(Serializer &out)
{
	Object::serialize(out);

	// shotelements
	{
		houdini::json::ArrayPtr elements = houdini::json::Array::create();
		for(auto it=m_elements.begin(), end=m_elements.end();it!=end;++it)
		{
			ShotElement::Ptr element = *it;
			elements->append(element->serialize(out));
		}
		out.write("elements", elements);
	}
	// updategraph
	{
		out.write("updateGraph", m_updateGraph->serialize( out ));
	}
}



houdini::json::Value ShotElement::serialize(Serializer &out)
{
	houdini::json::ObjectPtr obj = houdini::json::Object::create();

	obj->append( "element", out.serialize(m_element) );

	// childs
	{
		houdini::json::ArrayPtr childs = houdini::json::Array::create();
		for( auto it=m_childs.begin(), end=m_childs.end();it!=end;++it )
		{
			ShotElement::Ptr child = *it;
			childs->append( child->serialize(out) );
		}
		obj->append( "childs", childs );
	}

	return houdini::json::Value::createObject(obj);
}




REGISTERCLASS( Shot )







