/*---------------------------------------------------------------------

Contains nodes, connections and comments.

----------------------------------------------------------------------*/
#include "Graph.h"
/*
#include "Node.hpp"
#include "Pin.hpp"
#include "Connection.hpp"
*/
#include <QGraphicsSceneDragDropEvent>

namespace gui
{
	//
	// constructor
	//
	Graph::Graph( QObject *parent ) :
		QGraphicsScene(parent)
		//m_usePosHint(false)
	{
		setItemIndexMethod(QGraphicsScene::NoIndex);
		setSceneRect(-200, -200, 400, 400);
	}


	//
	// destructor
	//
	Graph::~Graph()
	{
	}


//	//
//	// adds a node to the graph - graph takes ownership of that node
//	//
//	void Graph::addNode( Node *node )
//	{
//		// is some position hint given ? (for example from a drag'n'drop operation)
//		if( m_usePosHint )
//		{
//			node->setPos(m_nextItemPos);
//			m_usePosHint = false;
//		}

//		// take ownership
//		node->setGraph(this);

//		// track nodes within the graph

//		// finally add it
//		QGraphicsScene::addItem( node );
//	}

//	//
//	// removes node from the graph - doesnt delete it!
//	//
//	void Graph::removeNode( Node *node )
//	{
//		// remove it
//		QGraphicsScene::removeItem(node);

//		node->setGraph(0);
//	}

//	//
//	// adds a connection between the specified output and input pins
//	//
//	void Graph::addConnection( Pin *out, Pin *in )
//	{
//		Connection *connection = new Connection(out, in);
//		addConnection( connection );
//	}

//	//
//	// adds a pre build connection
//	//
//	void Graph::addConnection( Connection *connection )
//	{
//		connection->getSourcePin()->registerConnection( connection );
//		connection->getDestPin()->registerConnection( connection );

//		// add item to qgraphicsscene
//		QGraphicsScene::addItem( connection );
//	}

//	//
//	// removes specified connection
//	//
//	void Graph::removeConnection( Connection *connection )
//	{
//		connection->getSourcePin()->unregisterConnection( connection );
//		connection->getDestPin()->unregisterConnection( connection );

//		// add item to qgraphicsscene
//		QGraphicsScene::removeItem( connection );
//	}


//	//
//	// handles drag-moves within a graph
//	// we generally allow drag movement since it is managed by the GraphWidget not by the graph.
//	//
//	// note: it is unclear why the dragdrop move event gets passed to the graph before it comes to the
//	// widget. having the widget informed via signals is a workaround
//	//
//	void Graph::dragMoveEvent( QGraphicsSceneDragDropEvent *event )
//	{
//		emit onDragMoveEvent(event);
//	}

//	//
//	// if set the next added node will be placed there
//	//
//	void Graph::givePlacingHint( QPointF &posHint )
//	{
//		m_nextItemPos = posHint;
//		m_usePosHint = true;
//	}

}
