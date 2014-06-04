//
//GraphWidget is a GUI element for editing node graphs like in mayas
//hypershade, mental mill etc.
//
#include "GraphWidget.h"

//#include "Graph.h"
//#include "Node.hpp"
/*
#include "GraphConnection.h"
#include "GraphNodeHandler.h"
#include "GraphNodePin.h"

#include <QDebug>
#include <QGraphicsScene>
#include <QWheelEvent>

#include <math.h>
*/


namespace gui
{
	GraphWidget::GraphWidget( QWidget *parent ) : QGraphicsView(parent)
	{
		/*
		setCacheMode(CacheBackground);
		setViewportUpdateMode(BoundingRectViewportUpdate);
		setRenderHint(QPainter::Antialiasing);
		setTransformationAnchor(AnchorUnderMouse);
		setResizeAnchor(AnchorViewCenter);

		setMinimumSize(400, 400);
		setWindowTitle(tr("GraphWidget"));

		scale( 1.1312f, 1.1312f );

		setAcceptDrops(true);

		// background color
		//setBackgroundBrush(QColor(225, 225, 225));
		setBackgroundBrush(QColor(125, 125, 125));



		//
		//m_virtualConnection = 0;
		//m_hoverPin          = 0;
		m_graph = 0;

		//m_currentState = NoScene;
		*/
	}

//	//
//	// sets the graph which is to be edited
//	//
//	void GraphWidget::assignGraph( Graph *graph )
//	{
//		// if we have currently connected a graph
//		if( m_graph )
//		{
//			// then we unconnect
//			//TODO: disconnect
//		}

//		// set the qt graphicsscene
//		setScene( graph );
//		// connect to the onDragMoveEvent (which is a workaround so that the widget gets
//		// to notice dragmoveevents)
//		connect( graph, SIGNAL(onDragMoveEvent(QGraphicsSceneDragDropEvent *)), this, SLOT(dragMoveEvent(QGraphicsSceneDragDropEvent *)) );

//		// keep track
//		m_graph = graph;
//	}

//	//
//	// returns the graph which is currently edited (may be null
//	//
//	Graph *GraphWidget::getGraph()
//	{
//		return m_graph;
//	}

//	//
//	// handles drag movement into the widget
//	//
//	void GraphWidget::dragEnterEvent(QDragEnterEvent* event)
//	{
//		printf("GraphWidget::dragEnterEvent\n");
//		emit onDragEnterEvent( event );
//	}


//	//
//	// handles drag movement within the widget
//	//
//	void GraphWidget::dragMoveEvent( QGraphicsSceneDragDropEvent * event )
//	{
//		printf("GraphWidget::dragMoveEvent\n");
//		emit onDragMoveEvent( event );
//	}

//	//
//	// handles dropping
//	//
//	void GraphWidget::dropEvent( QDropEvent *event )
//	{
//		printf("GraphWidget::dropEvent\n");
//		emit onDropEvent( event );
//	}

}
