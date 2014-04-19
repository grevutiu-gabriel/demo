#include "TransferFunctionItem.h"
#include "NodeItem.h"
#include "EdgeItem.h"

#include <QGraphicsScene>

namespace gui
{

QTransferFunctionItem::QTransferFunctionItem(QGraphicsItem* pParent) :
	QGraphicsRectItem(pParent),
	m_pTransferFunction(NULL),
	m_BrushEnabled(QBrush(QColor::fromHsl(0, 0, 170, 100))),
	m_BrushDisabled(QBrush(QColor::fromHsl(0, 0, 230, 100))),
	m_PolygonItem(this),
	m_Polygon(),
	m_Nodes(),
	m_Edges(),
	m_AllowUpdateNodes(true)
{
 	setBrush(Qt::NoBrush);
 	setPen(Qt::NoPen);

	m_PolygonItem.setPen(Qt::NoPen);
}

QTransferFunctionItem::~QTransferFunctionItem(void)
{
	for (int i = 0; i < m_Nodes.size(); i++)
	{
		scene()->removeItem(m_Nodes[i]);
		delete m_Nodes[i];
	}

	m_Nodes.clear();

	for (int i = 0; i < m_Edges.size(); i++)
	{
		scene()->removeItem(m_Edges[i]);
		delete m_Edges[i];
	}

	m_Edges.clear();
}

QTransferFunctionItem::QTransferFunctionItem(const QTransferFunctionItem& Other)
{
	*this = Other;
}

QTransferFunctionItem& QTransferFunctionItem::operator=(const QTransferFunctionItem& Other)
{
	m_pTransferFunction	= Other.m_pTransferFunction;
	m_BrushEnabled		= Other.m_BrushEnabled;
	m_BrushDisabled		= Other.m_BrushDisabled;
	m_Polygon			= Other.m_Polygon;
	m_Nodes				= Other.m_Nodes;
	m_Edges				= Other.m_Edges;
	m_AllowUpdateNodes	= Other.m_AllowUpdateNodes;

	return *this;
}

void QTransferFunctionItem::paint(QPainter* pPainter, const QStyleOptionGraphicsItem* pOption, QWidget* pWidget)
{
	if (isEnabled())
	{
		m_PolygonItem.setBrush(m_BrushEnabled);
	}
	else
	{
		m_PolygonItem.setBrush(m_BrushDisabled);
	}

	QGraphicsRectItem::paint(pPainter, pOption, pWidget);
}

void QTransferFunctionItem::SetTransferFunction(QTransferFunction *pTransferFunction)
{
	m_pTransferFunction = pTransferFunction;
}

QTransferFunction* QTransferFunctionItem::getTransferFunction()
{
	return m_pTransferFunction;
}

void QTransferFunctionItem::Update(void)
{
	UpdateNodes();
	UpdateEdges();
	UpdatePolygon();
}

void QTransferFunctionItem::UpdateNodes(void)
{
	if (!m_pTransferFunction || !m_AllowUpdateNodes)
		return;

	for (int i = 0; i < m_Nodes.size(); i++)
	{
		scene()->removeItem(m_Nodes[i]);
		delete m_Nodes[i];
	}

	m_Nodes.clear();

	for (int i = 0; i < m_pTransferFunction->GetNodes().size(); i++)
	{
		QNode& Node = m_pTransferFunction->GetNode(i);

		QPoint CanvasPoint;

		CanvasPoint.setX(Node.GetIntensity() * rect().width());
		CanvasPoint.setY((1.0f - Node.GetOpacity()) * rect().height());

		if (m_AllowUpdateNodes)
		{
			QNodeItem* pNodeItem = new QNodeItem(this, &m_pTransferFunction->GetNode(i));

			QPointF NodeCenter(Node.GetIntensity() * rect().width(), (1.0 - Node.GetOpacity()) * rect().height());

			pNodeItem->m_SuspendUpdate = true;

			pNodeItem->setPos(NodeCenter);
			pNodeItem->setZValue(100000);

			pNodeItem->m_SuspendUpdate = false;

			m_Nodes.append(pNodeItem);
		}
	}

	if (getTransferFunction()->GetSelectedNode())
	{
		for (int i = 0; i < m_Nodes.size(); i++)
		{
			if (m_Nodes[i]->m_pNode->GetID() == getTransferFunction()->GetSelectedNode()->GetID())
				m_Nodes[i]->setSelected(true);
		}
	}
}

void QTransferFunctionItem::UpdateEdges(void)
{
	if (!m_pTransferFunction)
		return;

	for (int i = 0; i < m_Edges.size(); i++)
	{
		scene()->removeItem(m_Edges[i]);
		delete m_Edges[i];
	}

	m_Edges.clear();

	QPoint CachedCanvasPoint;

	for (int i = 0; i < m_pTransferFunction->GetNodes().size(); i++)
	{
		QNode& Node = m_pTransferFunction->GetNode(i);

		QPoint CanvasPoint;

		CanvasPoint.setX(Node.GetIntensity() * rect().width());
		CanvasPoint.setY((1.0f - Node.GetOpacity()) * rect().height());

		if (i > 0)
		{
			QEdgeItem* pEdgeItem = new QEdgeItem(this);

			pEdgeItem->setLine(QLineF(CachedCanvasPoint, CanvasPoint));
			m_Edges.append(pEdgeItem);
		}

		CachedCanvasPoint = CanvasPoint;
	}
}

void QTransferFunctionItem::UpdatePolygon(void)
{
	if (!m_pTransferFunction)
		return;

	QLinearGradient LinearGradient;

	LinearGradient.setStart(0, rect().top());
	LinearGradient.setFinalStop(0, rect().bottom());

	QGradientStops GradientStopsEnabled;

	GradientStopsEnabled.append(QGradientStop(0, QColor::fromHsl(0, 0, 255, 150)));
	GradientStopsEnabled.append(QGradientStop(1, QColor::fromHsl(0, 0, 255, 15)));

	LinearGradient.setStops(GradientStopsEnabled);

	m_BrushEnabled = QBrush(LinearGradient);

	QGradientStops GradientStopsDisabled;

	GradientStopsDisabled.append(QGradientStop(0, QColor::fromHsl(0, 0, 255, 150)));
	GradientStopsDisabled.append(QGradientStop(1, QColor::fromHsl(0, 0, 255, 15)));

	LinearGradient.setStops(GradientStopsDisabled);

	m_BrushDisabled = QBrush(LinearGradient);

	m_Polygon.clear();

	for (int i = 0; i < m_pTransferFunction->GetNodes().size(); i++)
	{
		QNode& Node = m_pTransferFunction->GetNode(i);

		QPoint CanvasPoint;

		CanvasPoint.setX(Node.GetIntensity() * rect().width());
		CanvasPoint.setY((1.0f - Node.GetOpacity()) * rect().height());

		if (i == 0)
		{
			QPoint CanvasCopyPoint = CanvasPoint;
			CanvasCopyPoint.setY(rect().height());
			m_Polygon.append(CanvasCopyPoint);
		}

		m_Polygon.append(CanvasPoint);

		if (i == m_pTransferFunction->GetNodes().size() - 1)
		{
			CanvasPoint.setY(rect().height());
			m_Polygon.append(CanvasPoint);
		}
	}

	m_PolygonItem.setPolygon(m_Polygon);
}

} // namespace gui
