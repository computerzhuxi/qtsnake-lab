#pragma once
#include "Point.h"

/// \brief 食物，包含位置和分值
struct Food {
    Point position;
    int points = 1;
};
