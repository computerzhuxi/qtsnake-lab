#ifndef SNAKE_CORE_BOARD_H
#define SNAKE_CORE_BOARD_H

#include <cstdint>
#include <unordered_set>
#include <vector>
#include "Point.h"
#include "Snake.h"

/// \brief 棋盘地图（封装位掩码网格 + 空闲集）
/// \details 位掩码网格由 MoveComponent 通过 placeHead/removeTail 维护，
///          空闲集保留给 FoodComponent 用。所有字段私有，通过方法访问。
class Board {
public:
    /// \brief 单个格子的位掩码信息
    struct CellInfo {
        uint8_t snakeMask = 0;   // bit i = 第 i 条蛇有身体段在此
        uint8_t headMask  = 0;   // bit i = 第 i 条蛇的头在此
        bool hasFood      = false;
        bool hasObstacle  = false;
    };

    Board(int w = 20, int h = 20);

    // -- 尺寸 --
    int width() const  { return m_width; }
    int height() const { return m_height; }
    void setSize(int w, int h);

    // -- 初始构建（从蛇身填充 grid + freeCells） --
    void initFromSnakes(const std::vector<Snake>& snakes);

    // -- MoveComponent 调用 --
    void placeHead(int snakeIdx, Point pos);
    void placeBody(int snakeIdx, Point pos);
    void removeHead(int snakeIdx, Point pos);
    void removeTail(int snakeIdx, Point pos);

    // -- 查询 --
    bool isOutOfBounds(Point pos) const;
    const CellInfo& cellAt(Point pos) const;

    // -- 食物 --
    Point foodPos() const              { return m_foodPos; }
    void setFood(Point pos);
    void clearFood(Point pos);
    int foodPoints() const             { return m_foodPoints; }
    void setFoodPoints(int pts)        { m_foodPoints = pts; }
    bool isFoodEaten() const           { return m_foodEaten; }
    void markFoodEaten()               { m_foodEaten = true; }
    void clearFoodEaten()              { m_foodEaten = false; }

    // -- 空闲集 --
    const std::unordered_set<Point>& freeCells() const { return m_freeCells; }
    void addFreeCell(Point pos)        { m_freeCells.insert(pos); }
    void removeFreeCell(Point pos)     { m_freeCells.erase(pos); }

    // -- 障碍物（预留） --
    const std::unordered_set<Point>& obstacles() const { return m_obstacles; }
    void addObstacle(Point pos);

private:
    int m_width = 20;
    int m_height = 20;
    std::unordered_set<Point> m_freeCells;
    std::vector<std::vector<CellInfo>> m_grid;
    Point m_foodPos;
    int m_foodPoints = 1;
    bool m_foodEaten = false;
    std::unordered_set<Point> m_obstacles;

    void ensureGrid(int w, int h);
};

#endif // SNAKE_CORE_BOARD_H
