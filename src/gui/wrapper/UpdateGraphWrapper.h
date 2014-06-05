#pragma once

#include <unordered_set>
#include <QObject>
#include "../framework/UpdateGraph.h"
#include "ObjectWrapper.h"

namespace gui
{

	class UpdateGraphWrapper : public QObject
	{
		Q_OBJECT
	public:
		typedef std::shared_ptr<UpdateGraphWrapper> Ptr;

		UpdateGraphWrapper( UpdateGraph::Ptr graph );
		static Ptr create( UpdateGraph::Ptr graph );

		UpdateGraph::Ptr getUpdateGraph();

		void load();
		void getNodes( std::vector<ObjectWrapper::Ptr>& nodes );

		void addConnection( ObjectWrapper::Ptr controllerWrapper, ObjectWrapper::Ptr objectWrapper, const std::string& propName );
		void removeConnection( ObjectWrapper::Ptr controllerWrapper, ObjectWrapper::Ptr objectWrapper, const std::string& propName );

	signals:
	public slots:
	private:
		UpdateGraph::Ptr m_graph;
		std::unordered_set<ObjectWrapper::Ptr> m_nodes;
	};

} // namespace gui
