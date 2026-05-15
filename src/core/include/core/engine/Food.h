#ifndef CORE_ENGINE_FOOD_H
#define CORE_ENGINE_FOOD_H

#include "Point.h"
#include <random>

namespace core {

class Grid;
class Snake;

class Food {
public:
    Food();

    Point position() const { return pos_; }
    int value() const { return value_; }

    void respawn(const Grid& grid, const Snake& snake);
    void setPosition(Point p) { pos_ = p; }
    void setSeed(unsigned int seed);

private:
    Point pos_;
    int value_ = 1;
    std::mt19937 rng_;
};

} // namespace core

#endif // CORE_ENGINE_FOOD_H
