#ifndef CORE_ENGINE_SNAKE_H
#define CORE_ENGINE_SNAKE_H

#include "Direction.h"
#include "Point.h"
#include <deque>

namespace core {

class Snake {
public:
    Snake(Point startHead, int initialLength, Direction initialDir);

    void moveOneStep();
    void grow(int amount = 1);

    Point head() const;
    const std::deque<Point>& body() const;
    int length() const;

    Direction direction() const { return dir_; }
    void setDirection(Direction d);

    bool alive() const { return alive_; }
    void kill() { alive_ = false; }
    void revive() { alive_ = true; }

    void setBody(const std::deque<Point>& body, Direction dir);

private:
    std::deque<Point> body_;
    Direction dir_;
    int growthPending_ = 0;
    bool alive_ = true;

    Point nextHead() const;
};

} // namespace core

#endif // CORE_ENGINE_SNAKE_H
