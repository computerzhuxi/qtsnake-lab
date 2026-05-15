#ifndef CORE_AI_PATHRESULT_H
#define CORE_AI_PATHRESULT_H

#include "core/engine/Point.h"
#include <vector>

namespace core {

struct PathResult {
    std::vector<Point> path;
    bool found = false;
    int cost = 0;
};

} // namespace core

#endif // CORE_AI_PATHRESULT_H
