#pragma once

#include <QGraphicsRectItem>
#include <QGraphicsPolygonItem>
#include <QPolygon>
#include <QBrush>

#include "TransferFunction.h"
#include "../framework/elements/volumept/TransferFunction.h"

namespace gui
{
class QNodeItem;
class QEdgeItem;

class QTransferFunctionItem : public QGraphicsRectItem
{
public:
	QTransferFunctionItem(QGraphicsItem* pParent);
	virtual ~QTransferFunctionItem(void);
	QTransferFunctionItem::QTransferFunctionItem(const QTransferFunctionItem& Other);
	QTransferFunctionItem& operator = (const QTransferFunctionItem& Other);

	virtual void paint(QPainter* pPainter, const QStyleOptionGraphicsItem* pOption, QWidget* pWidget);

	void SetTransferFunction(QTransferFunction *pTransferFunction);
	QTransferFunction* getTransferFunction();
	void Update(void);
	void UpdateNodes(void);
	void UpdateEdges(void);
	void UpdatePolygon(void);

	
protected:
	QTransferFunction*		m_pTransferFunction;
	QBrush					m_BrushEnabled;
	QBrush					m_BrushDisabled;
	QGraphicsPolygonItem	m_PolygonItem;
	QPolygon				m_Polygon;
	QList<QNodeItem*>		m_Nodes;
	QList<QEdgeItem*>		m_Edges;
	bool					m_AllowUpdateNodes;

	friend class QNodeItem;
	friend class TFView;
};
}
