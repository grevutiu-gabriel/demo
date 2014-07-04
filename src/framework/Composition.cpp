#include "Composition.h"

base::Texture2d::Ptr Composition::m_noelement;
base::Texture2d::Ptr Composition::m_nocamera;

Composition::Composition() :
	ControllerT<RenderFunction>(),
	m_updateGraph(std::make_shared<UpdateGraph>())
{
	addProperty<base::Camera::Ptr>( "camera", std::bind( &Composition::getCamera, this ), std::bind( &Composition::setCamera, this, std::placeholders::_1 ) );
	addProperty("render", &m_renderElements);
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
	if( m_renderElements.empty() )
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
	for( auto renderElement:m_renderElements )
		if(renderElement)
			renderElement( context, time );
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


void Composition::serialize(Serializer &out)
{
	ControllerT<RenderFunction>::serialize(out);

	// updategraph
	{
		out.write("updateGraph", m_updateGraph->serialize( out ));
	}
	// render elements
	out.write("numRenderElements", int(m_renderElements.size()));
}

void Composition::deserialize(Deserializer &in)
{
	ControllerT<RenderFunction>::deserialize(in);

	// updategraph
	{
		m_updateGraph->deserialize( in, in.readValue("updateGraph") );
	}
	// render elements
	m_renderElements.resize( in.readInt("numRenderElements") );
}








RenderFunction Composition::evaluate(float time)
{
	//return std::bind( &Composition::render, this, std::placeholders::_1, std::placeholders::_2 );
	return RenderFunction();
}

REGISTERCLASS2( Composition, Controller )







