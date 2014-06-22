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

		Node&                                  addNode( ObjectWrapper::Ptr objectWrapper );
		void                                   addConnection( ObjectWrapper::Ptr controllerWrapper, ObjectWrapper::Ptr objectWrapper, const std::string& propName );
		void                                   removeConnection( ObjectWrapper::Ptr controllerWrapper, ObjectWrapper::Ptr objectWrapper, const std::string& propName );

		void                                   serialize( Serializer& out, houdini::json::ObjectPtr json );
		void                                   deserialize( Deserializer& in, houdini::json::ObjectPtr json );

		void                                   setPosition( ObjectWrapper::Ptr object, const QPointF& pos );
		QPointF                                getPosition(ObjectWrapper::Ptr object);
	signals:
		void propertyAdded( ObjectWrapper::Ptr objectWrapper, const std::string& prop );
		void propertyRemoved( ObjectWrapper::Ptr objectWrapper, const std::string& prop );
	public slots:
		void onObjectPropertyAdded( const std::string& name );
		void onObjectPropertyRemoved( const std::string& name );
	private:
		UpdateGraph::Ptr                       m_graph;
		std::map<ObjectWrapper::Ptr, Node>     m_nodes;
	};

} // namespace gui
