#ifndef SNAKE_UI_GAMESCENE_H
#define SNAKE_UI_GAMESCENE_H

#include <QGraphicsScene>
#include <QMap>
#include "SnakeItem.h"
#include "FoodItem.h"
#include "Snake.h"
#include "Board.h"

/// \brief 游戏场景：将 Board 模型数据同步为 QGraphicsItem 渲染
class GameScene : public QGraphicsScene {
    Q_OBJECT
public:
    explicit GameScene(QObject* parent = nullptr);
    void syncFromBoard(const Board& board, const std::vector<Snake*>& snakes);
    void clearAll();

private:
    QMap<int, QVector<SnakeItem*>> m_snakeItems;
    FoodItem* m_foodItem = nullptr;
    int cellToPixel(int cell) const;
};

#endif // SNAKE_UI_GAMESCENE_H
