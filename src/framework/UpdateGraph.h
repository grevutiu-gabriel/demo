#pragma once

#include <util/shared_ptr.h>
#include "houdini/json.h"
#include "Controller.h"

#include <iostream>



struct UpdateGraph
{
	typedef std::map<Property::Ptr, Controller::Ptr> ObjectBindings;

	void copyFrom( UpdateGraph& graph, Object::Ptr root );

	void clear()
	{
		m_updateCommands.clear();
		for(auto it = m_graph.begin(),end=m_graph.end();it!=end;++it)
		{
			ObjectBindings* bindings = it->second;
			delete bindings;
		}
		m_graph.clear();
	}

	void update( float time )
	{
		// iterate and execute all update commands
		for( auto it = m_updateCommands.begin(),end=m_updateCommands.end();it!=end;++it )
		{
			Controller::Ptr controller = it->first;
			Property::Ptr prop = it->second;
			controller->update(prop, time);
		}
	}

	ObjectBindings* getObjectBindings(Object::Ptr object)
	{
		auto it = m_graph.find(object);
		if(it != m_graph.end())
			return it->second;
		return 0;
	}

	void addConnection( Controller::Ptr controller, Object::Ptr object, const std::string& propName )
	{
		ObjectBindings* ob = getObjectBindings( object );
		// create, if objectbindings dont exist
		if(!ob)
		{
			ob = new ObjectBindings();
			m_graph[object] = ob;
		}
		Property::Ptr prop = object->getProperty(propName);
		if(prop)
			(*ob)[prop] = controller;
	}

	void gatherUpdateCommands( Object::Ptr object )
	{
		ObjectBindings* ob = getObjectBindings( object );
		if(ob)
		{
			for( auto it = ob->begin(), end=ob->end();it!=end;++it )
			{
				Property::Ptr property = it->first;
				Controller::Ptr controller = it->second;

				// recurse down the tree
				gatherUpdateCommands( controller );

				// now, since all leave nodes have been processed, add current binding
				m_updateCommands.push_back(std::make_pair(controller, property));
			}
		}
	}

	void compile()
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

	houdini::json::Value serialize(Serializer &out);

private:
	std::map<Object::Ptr, ObjectBindings* > m_graph; // the graph
	std::vector<std::pair<Controller::Ptr, Property::Ptr>> m_updateCommands; // this is the final list of update commands which is being compiled from the graph
};

