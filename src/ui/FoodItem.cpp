#include "FoodItem.h"
#include <QPainter>

FoodItem::FoodItem(QGraphicsItem* parent)
    : QGraphicsItem(parent)
{
}

QRectF FoodItem::boundingRect() const {
    int s = cellSize;
    return QRectF(-s / 2, -s / 2, s, s);
}

void FoodItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    int r = radius;
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor("#ff4466"));
    painter->drawEllipse(QPointF(0, 0), r, r);
}
