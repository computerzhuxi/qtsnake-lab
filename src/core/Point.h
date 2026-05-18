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
            size_t h1 = hash<int>()(p.x);
            size_t h2 = hash<int>()(p.y);
            return h1 ^ (h2 + 0x9e3779b97f4a7c15ULL + (h1 << 6) + (h1 >> 2));
        }
    };
}

#endif // SNAKE_CORE_POINT_H
