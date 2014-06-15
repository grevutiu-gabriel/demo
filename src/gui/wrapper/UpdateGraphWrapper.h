#pragma once

#include <QObject>
#include <QPoint>
#include <unordered_set>

#include "../framework/UpdateGraph.h"
#include "ObjectWrapper.h"

namespace gui
{

	class UpdateGraphWrapper : public QObject
	{
		Q_OBJECT
	public:
		typedef std::shared_ptr<UpdateGraphWrapper> Ptr;

		struct Node
		{
			QPointF pos;
		};

		UpdateGraphWrapper( UpdateGraph::Ptr graph );
		static Ptr                             create( UpdateGraph::Ptr graph );

		UpdateGraph::Ptr                       getUpdateGraph();

		void                                   load();
		void                                   getNodes( std::vector<ObjectWrapper::Ptr>& nodes );

		void                                   addNode( ObjectWrapper::Ptr objectWrapper );
		void                                   addConnection( ObjectWrapper::Ptr controllerWrapper, ObjectWrapper::Ptr objectWrapper, const std::string& propName );
		void                                   removeConnection( ObjectWrapper::Ptr controllerWrapper, ObjectWrapper::Ptr objectWrapper, const std::string& propName );

		void                                   serialize( Serializer& out, houdini::json::ObjectPtr json );
	signals:
	public slots:
	private:
		UpdateGraph::Ptr                       m_graph;
		std::map<ObjectWrapper::Ptr, Node>     m_nodes;
	};

} // namespace gui
