#ifndef SNAKE_UI_SNAKEITEM_H
#define SNAKE_UI_SNAKEITEM_H

#include <QGraphicsItem>
#include <QColor>

/// \brief 蛇身渲染项：方块造型+圆角+霓虹发光，头部高亮区分
class SnakeItem : public QGraphicsItem {
public:
    SnakeItem(bool isHead, QGraphicsItem* parent = nullptr);
    void setIsHead(bool isHead);
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;
    static constexpr int cellSize = 24;
    static constexpr int blockSize = 20;

private:
    bool m_isHead;
};

#endif // SNAKE_UI_SNAKEITEM_H
