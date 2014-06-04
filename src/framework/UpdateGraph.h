#pragma once

#include <util/shared_ptr.h>
#include "houdini/json.h"
#include "Controller.h"

#include <iostream>



class UpdateGraph
{
public:
	typedef std::shared_ptr<UpdateGraph> Ptr;
	typedef std::map<Property::Ptr, Controller::Ptr> ObjectBindings;

	UpdateGraph();
	UpdateGraph( UpdateGraph& graph );

	void copyFrom( UpdateGraph& graph, std::vector<Object::Ptr>& roots );
	void copyFrom( UpdateGraph& graph, Object::Ptr root );
	void copyFrom( UpdateGraph& graph, Object::Ptr root1, Object::Ptr root2 );

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

	void update( float time );

	void getNodes( std::vector<Object::Ptr>& nodes );

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

	void compile();

	houdini::json::Value serialize(Serializer &out);

private:
	std::map<Object::Ptr, ObjectBindings* > m_graph; // the graph
	std::vector<std::pair<Controller::Ptr, Property::Ptr>> m_updateCommands; // this is the final list of update commands which is being compiled from the graph
};

