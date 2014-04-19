
#include "EdgeItem.h"
#include "TransferFunction.h"
#include "TransferFunctionItem.h"

#include <QGraphicsItem>

namespace gui
{

QPen QEdgeItem::m_PenNormal		= QPen(QBrush(QColor::fromHsl(0, 100, 120)), 1.0);
QPen QEdgeItem::m_PenHighlight	= QPen(QBrush(QColor::fromHsl(0, 100, 120)), 1.0);
QPen QEdgeItem::m_PenDisabled	= QPen(QBrush(QColor::fromHsl(0, 0, 180)), 1.0);

QEdgeItem::QEdgeItem(QTransferFunctionItem* pTransferFunctionItem) :
	QGraphicsLineItem(pTransferFunctionItem),
	m_pTransferFunctionItem(pTransferFunctionItem)
{
	setPen(m_PenNormal);

	setParentItem(m_pTransferFunctionItem);
};

void QEdgeItem::paint(QPainter* pPainter, const QStyleOptionGraphicsItem* pOption, QWidget* pWidget)
{
	if (isEnabled())
	{
		if (isUnderMouse() || isSelected())
			setPen(m_PenHighlight);
		else
			setPen(m_PenNormal);
	}
	else
	{
		setPen(m_PenDisabled);
	}

	QGraphicsLineItem::paint(pPainter, pOption, pWidget);
}

} // namespace gui
