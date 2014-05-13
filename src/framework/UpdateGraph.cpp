#include "UpdateGraph.h"






void UpdateGraph::copyFrom( UpdateGraph& graph, Object::Ptr root )
{
	std::stack< Object::Ptr > todo;
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

}
