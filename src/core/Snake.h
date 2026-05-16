#ifndef SNAKE_CORE_SNAKE_H
#define SNAKE_CORE_SNAKE_H

#include <vector>
#include "Point.h"
#include "Direction.h"

/// \brief 蛇模型：管理身体段、移动、增长和方向控制
class Snake {
public:
    explicit Snake(Point start = Point{3, 3});
    void setDirection(Direction dir);
    Direction direction() const;
    void move();
    void grow();
    const std::vector<Point>& body() const;
    Point head() const;

private:
    std::vector<Point> m_body;
    Direction m_direction = Direction::Right;
    bool m_growNext = false;
};

#endif // SNAKE_CORE_SNAKE_H
