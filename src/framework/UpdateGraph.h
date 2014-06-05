#pragma once

#include <util/shared_ptr.h>
#include "houdini/json.h"
#include "Controller.h"

#include <iostream>



class UpdateGraph
{
public:
	typedef std::shared_ptr<UpdateGraph> Ptr;
	typedef std::map<std::string, Controller::Ptr> ObjectBindings;

	struct Connection
	{
		Object::Ptr src;
		Object::Ptr dest;
		std::string propName;
	};

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

	UpdateGraph::ObjectBindings* insertNode( Object::Ptr object );
	void getNodes( std::vector<Object::Ptr>& nodes );
	bool hasNode(Object::Ptr object);
	void getConnections( std::vector<Connection>& connections );

	ObjectBindings* getObjectBindings(Object::Ptr object);

	void addConnection( Controller::Ptr controller, Object::Ptr object, const std::string& propName );
	void removeConnection(Controller::Ptr controller, Object::Ptr object, const std::string& propName);

	void gatherUpdateCommands( Object::Ptr object );

	void compile();

	houdini::json::Value serialize(Serializer &out);

private:
	std::map<Object::Ptr, ObjectBindings* > m_graph; // the graph
	std::vector<std::pair<Controller::Ptr, Property::Ptr>> m_updateCommands; // this is the final list of update commands which is being compiled from the graph
};

