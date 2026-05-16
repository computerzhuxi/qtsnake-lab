#ifndef SNAKE_UI_FOODITEM_H
#define SNAKE_UI_FOODITEM_H

#include <QGraphicsItem>
#include <QColor>

/// \brief 食物渲染项：圆形+红色光晕
class FoodItem : public QGraphicsItem {
public:
    FoodItem(QGraphicsItem* parent = nullptr);
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;
    static constexpr int cellSize = 24;
    static constexpr int radius = 6;
};

#endif // SNAKE_UI_FOODITEM_H
