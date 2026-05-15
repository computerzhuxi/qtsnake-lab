#ifndef CORE_AI_IPATHFINDER_H
#define CORE_AI_IPATHFINDER_H

#include "core/ai/PathResult.h"
#include "core/engine/Point.h"

namespace core {

class Grid;
class Snake;

class IPathfinder {
public:
    virtual ~IPathfinder() = default;
    virtual PathResult findPath(const Grid& grid, const Snake& snake,
                                Point target) = 0;
};

} // namespace core

#endif // CORE_AI_IPATHFINDER_H
