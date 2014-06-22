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
				addConnection( controller, next, it->first );
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

UpdateGraph::ObjectBindings* UpdateGraph::insertNode(Object::Ptr object)
{
	ObjectBindings* ob = new ObjectBindings();
	m_graph[object] = ob;
	return ob;
}

void UpdateGraph::getNodes(std::vector<Object::Ptr> &nodes)
{
	for( auto it:m_graph )
		nodes.push_back(it.first);
}

bool UpdateGraph::hasNode(Object::Ptr object)
{
	return getObjectBindings(object)!=0;
}

void UpdateGraph::getConnections(std::vector<UpdateGraph::Connection>& connections)
{
	for( auto it:m_graph )
	{
		Object::Ptr obj = it.first;
		ObjectBindings* b = it.second;

		for( auto it2:*b )
		{
			Connection conn;
			conn.src = it2.second;
			conn.dest = obj;
			conn.propName = it2.first;
			connections.push_back(conn);
		}
	}
}

UpdateGraph::ObjectBindings *UpdateGraph::getObjectBindings(Object::Ptr object)
{
	auto it = m_graph.find(object);
	if(it != m_graph.end())
		return it->second;
	return 0;
}

void UpdateGraph::addConnection(Controller::Ptr controller, Object::Ptr object, const std::string &propName)
{
	ObjectBindings* ob = getObjectBindings( object );
	// create, if objectbindings dont exist
	if(!ob)
		ob = insertNode( object );
	(*ob)[propName] = controller;
	if(!hasNode( controller ))
		insertNode(controller);
}

void UpdateGraph::removeConnection(Controller::Ptr controller, Object::Ptr object, const std::string &propName)
{
	ObjectBindings* ob = getObjectBindings( object );
	if(ob)
	{
		auto it = ob->find(propName);
		if( it != ob->end() )
			ob->erase(it);
	}
}

void UpdateGraph::gatherUpdateCommands(Object::Ptr object)
{
	ObjectBindings* ob = getObjectBindings( object );
	if(ob)
	{
		for( auto it = ob->begin(), end=ob->end();it!=end;++it )
		{
			Property::Ptr property = object->getProperty(it->first);
			if(property)
			{
				Controller::Ptr controller = it->second;

				// recurse down the tree
				gatherUpdateCommands( controller );

				// now, since all leave nodes have been processed, add current binding
				m_updateCommands.push_back(std::make_pair(controller, property));
			}
		}
	}
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

		houdini::json::ArrayPtr jsonBindings = houdini::json::Array::create();
		for( auto it2 = bindings->begin(), end2=bindings->end();it2!=end2;++it2 )
		{
			houdini::json::ObjectPtr obj = houdini::json::Object::create();
			std::string propName = it2->first;
			Property::Ptr prop = object->getProperty(propName);
			Controller::Ptr controller = it2->second;
			obj->append( "controller",  out.serialize(controller));
			obj->appendValue<std::string>( "property",  prop->getName());
			jsonBindings->append(obj);
		}

		houdini::json::ObjectPtr json = houdini::json::Object::create();
		json->append("object", out.serialize(object));
		json->append("bindings", jsonBindings);

		jsonGraph->append(json);
	}

	return houdini::json::Value::createArray(jsonGraph);
}

void UpdateGraph::deserialize(Deserializer &in, houdini::json::Value value)
{
	houdini::json::ArrayPtr jsonGraph = value.asArray();

	for( int i=0,numElements=jsonGraph->size();i<numElements;++i )
	{
		houdini::json::ObjectPtr nodes = jsonGraph->getObject(i);

		Object::Ptr object = in.deserializeObject(nodes->getValue("object"));
		houdini::json::ArrayPtr jsonBindings = nodes->getArray("bindings");

		UpdateGraph::ObjectBindings* bindings = insertNode(object);

		for( int j=0,numElements2=jsonBindings->size();j<numElements2;++j )
		{
			houdini::json::ObjectPtr jsonBinding = jsonBindings->getObject(j);
			std::string propName = jsonBinding->get<std::string>("property");
			Controller::Ptr controller = std::dynamic_pointer_cast<Controller>(in.deserializeObject(jsonBinding->getValue("controller")));
			addConnection( controller, object, propName );
		}
	}
}

