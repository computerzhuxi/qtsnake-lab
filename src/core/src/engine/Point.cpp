#include "core/engine/Point.h"

namespace core {

bool Point::operator==(const Point& other) const {
    return x == other.x && y == other.y;
}

bool Point::operator!=(const Point& other) const {
    return !(*this == other);
}

Point Point::operator+(const Point& other) const {
    return {x + other.x, y + other.y};
}

Point& Point::operator+=(const Point& other) {
    x += other.x;
    y += other.y;
    return *this;
}

} // namespace core

namespace std {

size_t hash<core::Point>::operator()(const core::Point& p) const {
    return static_cast<size_t>(p.x) * 31 + static_cast<size_t>(p.y);
}

} // namespace std
