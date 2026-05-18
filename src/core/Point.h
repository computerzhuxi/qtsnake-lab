#ifndef SNAKE_CORE_POINT_H
#define SNAKE_CORE_POINT_H

#include <functional>

/// \brief 二维坐标点，表示棋盘上的一个格子位置
struct Point {
    int x = 0;
    int y = 0;

    Point() = default;
    Point(int x_, int y_) : x(x_), y(y_) {}

    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
    bool operator!=(const Point& other) const {
        return !(*this == other);
    }
};

/// \brief Point 的哈希特化，用于 std::unordered_set<Point>（空闲格列表）
namespace std {
    template<>
    struct hash<Point> {
        size_t operator()(const Point& p) const {
            return hash<int>()(p.x) ^ (hash<int>()(p.y) << 1);
        }
    };
}

#endif // SNAKE_CORE_POINT_H
