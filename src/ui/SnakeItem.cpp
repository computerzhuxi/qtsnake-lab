#include "SnakeItem.h"
#include <QPainter>

SnakeItem::SnakeItem(bool isHead, QGraphicsItem* parent)
    : QGraphicsItem(parent), m_isHead(isHead), m_color(isHead ? QColor("#00ff88") : QColor("#00bb55"))
{
}

void SnakeItem::setIsHead(bool isHead) {
    if (m_isHead != isHead) {
        m_isHead = isHead;
        update();
    }
}

void SnakeItem::setColor(const QColor& color) {
    if (m_color != color) {
        m_color = color;
        update();
    }
}

QRectF SnakeItem::boundingRect() const {
    double s = cellSize;
    // 蛇头需要额外的发光空间
    double glow = m_isHead ? 8.0 : 0.0;
    return QRectF(-s / 2 - glow, -s / 2 - glow, s + glow * 2, s + glow * 2);
}

void SnakeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    double bs = blockSize;
    double r = 4.0;

    if (m_isHead) {
        // 霓虹发光：4 层半透明矩形（由外向内）
        for (int layer = 3; layer >= 0; --layer) {
            int alpha = 40 - layer * 10;          // 40, 30, 20, 10
            double offset = (layer + 1) * 2.0;    // 8, 6, 4, 2
            painter->setPen(Qt::NoPen);
            painter->setBrush(QColor(0, 255, 136, alpha));
            painter->drawRoundedRect(QRectF(-bs / 2 - offset, -bs / 2 - offset,
                                            bs + offset * 2, bs + offset * 2),
                                     r + 1, r + 1);
        }
        // 主体
        painter->setPen(QPen(QColor("#44ffaa"), 1.0));
        painter->setBrush(m_color);
    } else {
        painter->setPen(QPen(QColor("#22cc66"), 1.0));
        painter->setBrush(m_color);
    }
    painter->drawRoundedRect(QRectF(-bs / 2, -bs / 2, bs, bs), r, r);
}
