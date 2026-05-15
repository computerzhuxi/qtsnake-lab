#include "core/engine/Snake.h"
#include <algorithm>

namespace core {

Snake::Snake(Point startHead, int initialLength, Direction initialDir)
    : dir_(initialDir)
{
    body_.push_back(startHead);
    Direction tailDir = opposite(initialDir);
    Point delta;
    switch (tailDir) {
    case Direction::Up:    delta = {0, -1}; break;
    case Direction::Down:  delta = {0, 1}; break;
    case Direction::Left:  delta = {-1, 0}; break;
    case Direction::Right: delta = {1, 0}; break;
    }

    Point current = startHead;
    for (int i = 1; i < initialLength; ++i) {
        current = current + delta;
        body_.push_back(current);
    }
}

Point Snake::nextHead() const {
    Point h = head();
    switch (dir_) {
    case Direction::Up:    return {h.x, h.y - 1};
    case Direction::Down:  return {h.x, h.y + 1};
    case Direction::Left:  return {h.x - 1, h.y};
    case Direction::Right: return {h.x + 1, h.y};
    }
    return h;
}

void Snake::moveOneStep() {
    if (!alive_) return;

    body_.push_front(nextHead());
    if (growthPending_ > 0) {
        --growthPending_;
    } else {
        body_.pop_back();
    }
}

void Snake::grow(int amount) {
    growthPending_ += amount;
}

Point Snake::head() const {
    return body_.front();
}

const std::deque<Point>& Snake::body() const {
    return body_;
}

int Snake::length() const {
    return static_cast<int>(body_.size());
}

void Snake::setDirection(Direction d) {
    if (d != opposite(dir_)) {
        dir_ = d;
    }
}

void Snake::setBody(const std::deque<Point>& body, Direction dir) {
    body_ = body;
    dir_ = dir;
    growthPending_ = 0;
    alive_ = true;
}

} // namespace core
