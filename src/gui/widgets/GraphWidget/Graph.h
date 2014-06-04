//
//Contains nodes, connections and comments.
//
#pragma once

#include <QGraphicsScene>

namespace gui
{
	//class Pin;
	//class Node;
	//class Connection;

	//
	// Contains nodes, connections and comments.
	//
	class Graph : public QGraphicsScene
	{
		Q_OBJECT
	public:

		Graph( QObject *parent = 0 ); // constructor
		~Graph();                     // destructor

		/*
		void                                               addNode( Node *node ); // adds a node to the graph - graph takes ownership of that node
		void                                            removeNode( Node *node ); // removes node from the graph - doesnt delete it!
		void                                  addConnection( Pin *out, Pin *in ); // adds a connection between the specified output and input pins
		void                             addConnection( Connection *connection ); // adds a pre build connection
		void                          removeConnection( Connection *connection ); // removes specified connection
		void                                 givePlacingHint( QPointF &posHint ); // if set the next added node will be placed there

	signals:
		 //void                                      onDragEnterEvent( QDragEnterEvent* event);  // will be emitted on a drag enter event
 		 void                          onDragMoveEvent( QGraphicsSceneDragDropEvent *event );  // will be emitted on a dragmove event
		 //void                                               onDropEvent( QDropEvent *event );  // will be emitted on a drop event


	private:
		// QGraphicsScene overloads
		virtual void         dragMoveEvent( QGraphicsSceneDragDropEvent *event );  // has to be overwritten so that dragndrop works

		bool                                                        m_usePosHint;  // if true then the m_nextItemPos is used to place the next incoming item
		QPointF                                                    m_nextItemPos;
		*/
	};
}
