#include "SnakeItem.h"
#include <QPainter>

SnakeItem::SnakeItem(bool isHead, QGraphicsItem* parent)
    : QGraphicsItem(parent), m_isHead(isHead)
{
}

QRectF SnakeItem::boundingRect() const {
    int s = cellSize;
    return QRectF(-s / 2, -s / 2, s, s);
}

void SnakeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    int s = blockSize;
    int r = 4;

    if (m_isHead) {
        painter->setPen(QPen(QColor("#44ffaa"), 1));
        painter->setBrush(QColor("#00ff88"));
    } else {
        painter->setPen(QPen(QColor("#22cc66"), 1));
        painter->setBrush(QColor("#00bb55"));
    }
    painter->drawRoundedRect(QRectF(-s / 2.0, -s / 2.0, s, s), r, r);
}
