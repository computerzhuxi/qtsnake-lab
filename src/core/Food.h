#ifndef SNAKE_CORE_FOOD_H
#define SNAKE_CORE_FOOD_H

#include "Point.h"

/// \brief 食物，包含位置和分值
struct Food {
    Point position;
    int points = 1;
};

#endif // SNAKE_CORE_FOOD_H
