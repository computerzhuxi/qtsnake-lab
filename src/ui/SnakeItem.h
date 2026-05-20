#ifndef SNAKE_UI_SNAKEITEM_H
#define SNAKE_UI_SNAKEITEM_H

#include <QGraphicsItem>
#include <QColor>

/// \brief 蛇身渲染项
/// \details 方块造型（20×20）+ 4px 圆角 + 蛇头霓虹发光。
///          颜色由 GameScene 逐段计算后通过 setColor() 传入。
///          在 24×24 的格子内居中绘制。

class SnakeItem : public QGraphicsItem {
public:
    SnakeItem(bool isHead, QGraphicsItem* parent = nullptr);
    void setIsHead(bool isHead);
    void setColor(const QColor& color);
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;

    static constexpr int cellSize = 24;     ///< 格子尺寸（像素）
    static constexpr int blockSize = 20;    ///< 蛇身方块尺寸（像素）

private:
    bool m_isHead;                          ///< 是否为蛇头
    QColor m_color;                         ///< 当前段颜色
};

#endif // SNAKE_UI_SNAKEITEM_H
