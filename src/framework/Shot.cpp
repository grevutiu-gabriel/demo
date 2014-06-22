#include "Shot.h"

base::Texture2d::Ptr Shot::m_noelement;
base::Texture2d::Ptr Shot::m_nocamera;

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
	if( m_elements.empty() )
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if( !m_noelement )
			m_noelement = base::Texture2d::load(base::expand("$DATA/framework/noelement.png"));
		context->renderScreen(m_noelement);
		return;
	}

	// update all properties ---
	m_updateGraph->update(time);

	// get camera ---
	base::Camera::Ptr camera = m_camera;
	if( overrideCamera )
		camera = overrideCamera;

	if( camera )
		context->setView( camera->m_worldToView, camera->m_viewToWorld, camera->m_viewToNDC );
	else
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if( !m_nocamera )
			m_nocamera = base::Texture2d::load(base::expand("$DATA/framework/nocamera.png"));
		context->renderScreen(m_nocamera);
		return;
	}

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

ShotElement::Ptr Shot::takeShotElement(int index)
{
	ShotElement::Ptr se = getShotElement(index);
	m_elements.erase(m_elements.begin()+index);
	return se;
}

void Shot::insertElement(int index, ShotElement::Ptr shotElement )
{
	m_elements.insert( m_elements.begin()+index, shotElement );
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

void Shot::deserialize(Deserializer &in)
{
	Object::deserialize(in);
	// shotelements
	{
		houdini::json::ArrayPtr shotElements = in.readArray("elements");
		for(int i=0,numElements=shotElements->size();i<numElements;++i)
		{
			ShotElement::Ptr shotElement = std::make_shared<ShotElement>();
			shotElement->deserialize(in, shotElements->getValue(i));
			addElement(shotElement);
		}
	}
	// updategraph
	{
		m_updateGraph->deserialize( in, in.readValue("updateGraph") );
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

void ShotElement::deserialize(Deserializer &in, houdini::json::Value value)
{
	houdini::json::ObjectPtr obj = value.asObject();
	m_element = std::dynamic_pointer_cast<Element>( in.deserializeObject(obj->getValue("element")) );
	// childs
	{
		houdini::json::ArrayPtr childs = obj->getArray( "childs" );
		for( int i=0,numElements=childs->size();i<numElements;++i )
		{
			ShotElement::Ptr child = std::make_shared<ShotElement>();
			child->deserialize(in, childs->getValue(i));
			m_childs.push_back(child);
		}
	}
}




REGISTERCLASS( Shot )







