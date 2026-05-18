#ifndef SNAKE_UI_FOODITEM_H
#define SNAKE_UI_FOODITEM_H

#include <QGraphicsItem>
#include <QColor>

/// \brief 食物渲染项
/// \details 圆形（半径 6px）+ 红色 #ff4466 填充 + 无边线。
///          在 24×24 的格子内居中绘制。

class FoodItem : public QGraphicsItem {
public:
    FoodItem(QGraphicsItem* parent = nullptr);
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;

    static constexpr int cellSize = 24;     ///< 格子尺寸（像素）
    static constexpr int radius = 6;        ///< 食物圆半径（像素）
};

#endif // SNAKE_UI_FOODITEM_H
