//
//GraphWidget is a GUI element for editing node graphs like mayas
//hypershade, mental mill etc.
//
#pragma once


#include <QGraphicsView>


namespace gui
{
	//class Graph;
	//class  Node;
	//class GraphNodePin;
	//class GraphConnection;

	//
	// GraphWidget is a GUI element for editing node graphs
	//
	class GraphWidget : public QGraphicsView
	{
		Q_OBJECT
	public:
//		enum State
//		{
//			NoScene,          // current scene is null nothing is edited
//			Edit,             // default state
//			DrawConnection    // this state is active when a user drags a connection around to connect some input with a output
//		};

		GraphWidget( QWidget *parent = 0 );      // constructor

		//void                   assignGraph( Graph *graph );  // sets the graph which is to be edited
		//Graph*                 getGraph();  // returns the graph which is currently edited (may be null)

	/*
		void                                                                     itemMoved();  // 

		void             enterDrawConnectionState( GraphNodePin *output, QPointF destPoint );  // user clicked into a pin shape and is starting to draw a new connection
		void                                                      leaveDrawConnectionState();  // leaves the draw connection state
		void                                                                enterEditState();  // enters edit state
	*/
	/*
	signals:
		 void                                      onDragEnterEvent( QDragEnterEvent* event);  // will be emitted on a drag enter event
 		 void                          onDragMoveEvent( QGraphicsSceneDragDropEvent *event );  // will be emitted on a dragmove event
		 void                                               onDropEvent( QDropEvent *event );  // will be emitted on a drop event
	*/

	//public slots:
	//	
	protected slots:
		//QWidget overrides
		//void                                               keyPressEvent(QKeyEvent *event);  // handles key press events
		//void                                                wheelEvent(QWheelEvent *event);  // handles mouse-wheel events
		//void                                            mouseMoveEvent(QMouseEvent *event);  // handles mousemove events
		//void                                         mouseReleaseEvent(QMouseEvent *event);  // handles mouserelease events
		//void                                          dragEnterEvent(QDragEnterEvent* event);  // handles drag movement into the widget
		//void                            dragMoveEvent( QGraphicsSceneDragDropEvent * event );  // handles drag movement within the widget
		//void                                                  dropEvent( QDropEvent *event );  // handles dropping
		//void                         drawBackground(QPainter *painter, const QRectF &rect);  // draws the background
	protected:
	/*
		State                                                               m_currentState;  //

		// used in DrawConnectin state
		GraphConnection                                               *m_virtualConnection;
		GraphNodePin                                                           *m_hoverPin;
	*/
		//Graph                                                                       *m_graph;  // current graph which is being edited

	};

}
