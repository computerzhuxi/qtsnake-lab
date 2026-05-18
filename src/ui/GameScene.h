#ifndef SNAKE_UI_GAMESCENE_H
#define SNAKE_UI_GAMESCENE_H

#include <QGraphicsScene>
#include <QMap>
#include "SnakeItem.h"
#include "FoodItem.h"
#include "GameState.h"

/// \brief 游戏场景：将 GameState 数据同步为 QGraphicsItem 画面
/// \details 采用增量更新策略：蛇段数不变→setPos/setIsHead，增长→new，减少→delete。

class GameScene : public QGraphicsScene {
    Q_OBJECT
public:
    explicit GameScene(QObject* parent = nullptr);

    /// \brief 同步 GameState 数据到画面
    void syncFromState(const GameState& state);

    /// \brief 清空所有 item
    void clearAll();

private:
    QMap<int, QVector<SnakeItem*>> m_snakeItems;
    FoodItem* m_foodItem = nullptr;
    QGraphicsRectItem* m_borderItem = nullptr;
    int m_boardW = 20;
    int m_boardH = 20;
    int cellToPixel(int cell) const;
};

#endif // SNAKE_UI_GAMESCENE_H
