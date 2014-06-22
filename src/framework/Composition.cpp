#include "Composition.h"

base::Texture2d::Ptr Composition::m_noelement;
base::Texture2d::Ptr Composition::m_nocamera;

Composition::Composition() :
	CompositionElement(),
	m_updateGraph(std::make_shared<UpdateGraph>())
{
	addProperty<base::Camera::Ptr>( "camera", std::bind( &Composition::getCamera, this ), std::bind( &Composition::setCamera, this, std::placeholders::_1 ) );
}

Composition::Ptr Composition::create()
{
	return std::make_shared<Composition>();
}


void Composition::prepareForRendering()
{
	m_updateGraph->compile();
}

void Composition::render( base::Context::Ptr context, float time, base::Camera::Ptr overrideCamera )
{
	if( m_childs.empty() )
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
	for( auto it = m_childs.begin(), end=m_childs.end();it!=end;++it )
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

base::Camera::Ptr Composition::getCamera() const
{
	return m_camera;
}

void Composition::setCamera(base::Camera::Ptr camera)
{
	m_camera = camera;
}
/*
int Composition::getNumCompositionElements() const
{
	return int(m_elements.size());
}





void Composition::insertElement(int index, CompositionElement::Ptr compositionElement )
{
	m_elements.insert( m_elements.begin()+index, compositionElement );
}

std::vector<CompositionElement::Ptr> &Composition::getCompositionElements()
{
	return m_elements;
}
*/


void Composition::serialize(Serializer &out)
{
	CompositionElement::serialize(out);

	// updategraph
	{
		out.write("updateGraph", m_updateGraph->serialize( out ));
	}
}

void Composition::deserialize(Deserializer &in)
{
	CompositionElement::deserialize(in);

	// updategraph
	{
		m_updateGraph->deserialize( in, in.readValue("updateGraph") );
	}
}






REGISTERCLASS2( Composition, CompositionElement )







