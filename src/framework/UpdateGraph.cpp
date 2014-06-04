#include "UpdateGraph.h"





UpdateGraph::UpdateGraph()
{
}

UpdateGraph::UpdateGraph(UpdateGraph &graph):
	m_graph(graph.m_graph)
{

}

void UpdateGraph::copyFrom(UpdateGraph &graph, std::vector<Object::Ptr> &roots)
{
	std::stack< Object::Ptr > todo;
	for( auto root:roots )
		todo.push(root);

	while( !todo.empty() )
	{
		Object::Ptr next = todo.top();
		todo.pop();
		ObjectBindings* src_bindings = graph.getObjectBindings(next);
		if(src_bindings)
		{
			for( auto it = src_bindings->begin(),end=src_bindings->end();it!=end;++it )
			{
				Controller::Ptr controller = it->second;
				Property::Ptr prop = it->first;
				addConnection( controller, next, prop->getName() );
				todo.push( controller );
			}
		}
	}

	compile();
}

void UpdateGraph::copyFrom( UpdateGraph& graph, Object::Ptr root )
{
	std::vector<Object::Ptr> roots;
	roots.push_back(root);
	copyFrom(graph, roots);
}

void UpdateGraph::copyFrom(UpdateGraph &graph, Object::Ptr root1, Object::Ptr root2)
{
	std::vector<Object::Ptr> roots;
	roots.push_back(root1);
	roots.push_back(root2);
	copyFrom(graph, roots);
}

void UpdateGraph::update(float time)
{
	//std::cout << "UpdateGraph::update" << std::endl;
	// iterate and execute all update commands
	for( auto it = m_updateCommands.begin(),end=m_updateCommands.end();it!=end;++it )
	{
		Controller::Ptr controller = it->first;
		Property::Ptr prop = it->second;
		controller->update(prop, time);
		//std::cout << "updating: " << prop->getName() << " from controller: " << controller->getMetaObject()->getTypeName() << std::endl;
	}
}

void UpdateGraph::getNodes(std::vector<Object::Ptr> &nodes)
{
	for( auto it:m_graph )
		nodes.push_back(it.first);
}

void UpdateGraph::compile()
{
	m_updateCommands.clear();

	// find root objects ---
	std::vector<Object::Ptr> rootObjects;

	// iterate all graph nodes
	std::map<Object::Ptr, bool>  isChild;
	for( auto it = m_graph.begin(), end=m_graph.end();it!=end;++it )
	{
		Object::Ptr obj = it->first;
		if( isChild.find(obj)==isChild.end() )
			isChild[obj] = false;
		ObjectBindings* bindings = it->second;
		for( auto it2 = bindings->begin(),end2 = bindings->end();it2!=end2;++it2 )
		{
			Object::Ptr child = it2->second;
			isChild[child] = true;
		}
	}
	for( auto it = isChild.begin(), end=isChild.end();it!=end;++it )
	{
		Object::Ptr obj = it->first;
		bool objIsChild = it->second;
		if(!objIsChild)
			rootObjects.push_back(obj);
	}

	// now gather update commands in the correct order ---
	for( auto it=rootObjects.begin(), end=rootObjects.end();it!=end;++it )
	{
		Object::Ptr obj = *it;
		gatherUpdateCommands( obj );
	}
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

