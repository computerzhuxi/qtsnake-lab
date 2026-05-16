#ifndef SNAKE_CORE_POINT_H
#define SNAKE_CORE_POINT_H

/// \brief 二维坐标点，表示棋盘上的一个格子位置
struct Point {
    int x = 0;
    int y = 0;

    Point() = default;
    Point(int x_, int y_) : x(x_), y(y_) {}

    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

#endif // SNAKE_CORE_POINT_H
