#include "Shot.h"





void Shot::prepareForRendering()
{
	// find all root objects of the updategraph ---
	std::vector<Element::Ptr> elements;
	for(auto it = m_elements.begin(),end=m_elements.end();it!=end;++it)
	{
		auto shotElement = *it;
		shotElement->getAllChildElements(elements);
	}
	// upcast elements---
	std::vector<Object::Ptr> rootObjects;
	for(auto it = elements.begin(),end=elements.end();it!=end;++it)
		rootObjects.push_back(*it);
	// now compile the graph with given root objects
	m_updateGraph.compile(rootObjects);
}

void Shot::render( base::Context::Ptr context, float time, base::Camera::Ptr overrideCamera )
{
//		// update animated properties
//		for(auto it=m_constantProperties.begin(),end=m_constantProperties.end();it!=end;++it)
//			it->second->update( it->first, time);
//		for(auto it=m_animatedProperties.begin(),end=m_animatedProperties.end();it!=end;++it)
//			it->second->update( it->first, time);

	// update all properties ---
	m_updateGraph.update(time);

	// get camera ---
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
		out.write("updateGraph", m_updateGraph.serialize( out ));
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



houdini::json::Value UpdateGraph::serialize(Serializer &out)
{
	houdini::json::ArrayPtr jsonGraph = houdini::json::Array::create();

	// serialize graph: objectid->object bindings
	for( auto it = m_graph.begin(), end=m_graph.end();it!=end;++it )
	{
		Object::Ptr object = it->first;
		ObjectBindings* bindings = it->second;

		houdini::json::ObjectPtr jsonBindings = houdini::json::Object::create();
		for( auto it2 = bindings->begin(), end2=bindings->end();it2!=end2;++it2 )
		{
			Property::Ptr prop = it2->first;
			Controller::Ptr controller = it2->second;
			jsonBindings->append( "controller",  out.serialize(controller));
			jsonBindings->appendValue<std::string>( "property",  prop->getName());
		}

		houdini::json::ObjectPtr json = houdini::json::Object::create();
		json->append("object", out.serialize(object));
		json->append("bindings", jsonBindings);

		jsonGraph->append(json);
	}

	return houdini::json::Value::createArray(jsonGraph);
}

REGISTERCLASS( Shot )







