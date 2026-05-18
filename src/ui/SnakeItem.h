#ifndef SNAKE_UI_SNAKEITEM_H
#define SNAKE_UI_SNAKEITEM_H

#include <QGraphicsItem>
#include <QColor>

/// \brief 蛇身渲染项
/// \details 方块造型（20×20）+ 4px 圆角 + 霓虹发光。
///          头部用亮绿色 #00ff88，身体用暗绿色 #00bb55，一眼区分朝向。
///          在 24×24 的格子内居中绘制。支持 setIsHead() 复用切换头/身。

class SnakeItem : public QGraphicsItem {
public:
    SnakeItem(bool isHead, QGraphicsItem* parent = nullptr);
    void setIsHead(bool isHead);
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;

    static constexpr int cellSize = 24;     ///< 格子尺寸（像素）
    static constexpr int blockSize = 20;    ///< 蛇身方块尺寸（像素）

private:
    bool m_isHead;                          ///< 是否为蛇头（true=亮绿, false=暗绿）
};

#endif // SNAKE_UI_SNAKEITEM_H
