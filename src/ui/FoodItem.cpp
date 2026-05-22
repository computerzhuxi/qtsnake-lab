#include "FoodItem.h"
#include <QPainter>

FoodItem::FoodItem(QGraphicsItem* parent)
    : QGraphicsItem(parent)
{
}

QRectF FoodItem::boundingRect() const {
    double glow = 9.0;
    double s = cellSize + glow * 2;
    return QRectF(-s / 2, -s / 2, s, s);
}

void FoodItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    int r = radius;

    // 光晕：3 层同心圆（由外向内）
    for (int layer = 2; layer >= 0; --layer) {
        int alpha = 35 - layer * 12;            // 35, 23, 11
        int glowR = r + (layer + 1) * 3;        // 15, 12, 9
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(0xff, 0x44, 0x66, alpha));
        painter->drawEllipse(QPointF(0, 0), glowR, glowR);
    }
    // 主体
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor("#ff4466"));
    painter->drawEllipse(QPointF(0, 0), r, r);
}
