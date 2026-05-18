#ifndef SNAKE_CORE_SNAKE_H
#define SNAKE_CORE_SNAKE_H

#include <vector>
#include "Point.h"
#include "Direction.h"

/// \brief 蛇模型（数据 + 自身行为）
/// \details 管理身体段、移动、增长、方向控制。
///          碰撞检测由 CollisionComponent 通过空闲集统一处理。
///          move() 在头部插入新坐标，尾部弹出（growNext 时除外）。
///          不做边界检测——边界由 Controller 处理。

class Snake {
public:
    Snake() : Snake(Point{3, 3}) {}

    /// \param start 头部初始位置
    /// \param initialDir 初始方向。身体段往反方向延伸 2 格。
    explicit Snake(Point start, Direction initialDir = Direction::Right);

    void setDirection(Direction dir);
    Direction direction() const;
    void move();
    void grow();

    const std::vector<Point>& body() const;
    Point head() const;

    /// \brief grow() 是否已被调用但尚未在 move() 中生效
    bool growPending() const { return m_growNext; }

private:
    std::vector<Point> m_body;
    Direction m_direction = Direction::Right;
    bool m_growNext = false;
};

#endif // SNAKE_CORE_SNAKE_H
