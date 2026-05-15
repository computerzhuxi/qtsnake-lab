#ifndef CORE_ENGINE_POINT_H
#define CORE_ENGINE_POINT_H

#include <cstdint>
#include <functional>

namespace core {

struct Point {
    int x = 0;
    int y = 0;

    bool operator==(const Point& other) const;
    bool operator!=(const Point& other) const;

    Point operator+(const Point& other) const;
    Point& operator+=(const Point& other);
};

} // namespace core

namespace std {
template <>
struct hash<core::Point> {
    size_t operator()(const core::Point& p) const;
};
} // namespace std

#endif // CORE_ENGINE_POINT_H
