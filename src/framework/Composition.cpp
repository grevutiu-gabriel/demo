#include "Composition.h"

base::Texture2d::Ptr Composition::m_noelement;
base::Texture2d::Ptr Composition::m_nocamera;

Composition::Composition() :
	Object(),
	m_updateGraph(std::make_shared<UpdateGraph>())
{
	addProperty<base::Camera::Ptr>( "camera", std::bind( &Composition::getCamera, this ), std::bind( &Composition::setCamera, this, std::placeholders::_1 ) );
}


void Composition::prepareForRendering()
{
	m_updateGraph->compile();
}

void Composition::render( base::Context::Ptr context, float time, base::Camera::Ptr overrideCamera )
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
		CompositionElement::Ptr compositionElement = *it;
		compositionElement->render(context, time);
	}
}

void Composition::setPropertyController(Object::Ptr object, const std::string &name, Controller::Ptr controller)
{
	m_updateGraph->addConnection( controller, object, name );
}

UpdateGraph::Ptr Composition::getUpdateGraph()
{
	return m_updateGraph;
}

int Composition::getNumCompositionElements() const
{
	return int(m_elements.size());
}

CompositionElement::Ptr Composition::getCompositionElement( int index )
{
	return m_elements[index];
}

CompositionElement::Ptr Composition::takeCompositionElement(int index)
{
	CompositionElement::Ptr se = getCompositionElement(index);
	m_elements.erase(m_elements.begin()+index);
	return se;
}

void Composition::insertElement(int index, CompositionElement::Ptr compositionElement )
{
	m_elements.insert( m_elements.begin()+index, compositionElement );
}

std::vector<CompositionElement::Ptr> &Composition::getCompositionElements()
{
	return m_elements;
}



void Composition::serialize(Serializer &out)
{
	Object::serialize(out);

	// compositionelements
	{
		houdini::json::ArrayPtr elements = houdini::json::Array::create();
		for(auto it=m_elements.begin(), end=m_elements.end();it!=end;++it)
		{
			CompositionElement::Ptr element = *it;
			elements->append(element->serialize(out));
		}
		out.write("elements", elements);
	}
	// updategraph
	{
		out.write("updateGraph", m_updateGraph->serialize( out ));
	}
}

void Composition::deserialize(Deserializer &in)
{
	Object::deserialize(in);
	// compositionelements
	{
		houdini::json::ArrayPtr compositionElements = in.readArray("elements");
		for(int i=0,numElements=compositionElements->size();i<numElements;++i)
		{
			CompositionElement::Ptr compositionElement = std::make_shared<CompositionElement>();
			compositionElement->deserialize(in, compositionElements->getValue(i));
			addElement(compositionElement);
		}
	}
	// updategraph
	{
		m_updateGraph->deserialize( in, in.readValue("updateGraph") );
	}
}



houdini::json::Value CompositionElement::serialize(Serializer &out)
{
	houdini::json::ObjectPtr obj = houdini::json::Object::create();

	obj->append( "element", out.serialize(m_element) );

	// childs
	{
		houdini::json::ArrayPtr childs = houdini::json::Array::create();
		for( auto it=m_childs.begin(), end=m_childs.end();it!=end;++it )
		{
			CompositionElement::Ptr child = *it;
			childs->append( child->serialize(out) );
		}
		obj->append( "childs", childs );
	}

	return houdini::json::Value::createObject(obj);
}

void CompositionElement::deserialize(Deserializer &in, houdini::json::Value value)
{
	houdini::json::ObjectPtr obj = value.asObject();
	m_element = std::dynamic_pointer_cast<Element>( in.deserializeObject(obj->getValue("element")) );
	// childs
	{
		houdini::json::ArrayPtr childs = obj->getArray( "childs" );
		for( int i=0,numElements=childs->size();i<numElements;++i )
		{
			CompositionElement::Ptr child = std::make_shared<CompositionElement>();
			child->deserialize(in, childs->getValue(i));
			m_childs.push_back(child);
		}
	}
}




REGISTERCLASS( Composition )







