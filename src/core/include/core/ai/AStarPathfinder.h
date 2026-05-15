#ifndef CORE_AI_ASTARPATHFINDER_H
#define CORE_AI_ASTARPATHFINDER_H

#include "core/ai/IPathfinder.h"
#include <unordered_set>
#include <vector>

namespace core {

class AStarPathfinder : public IPathfinder {
public:
    PathResult findPath(const Grid& grid, const Snake& snake,
                        Point target) override;

    static int manhattan(Point a, Point b);

private:
    struct Node {
        Point pos;
        int g = 0; // cost from start
        int h = 0; // heuristic to target
        int f() const { return g + h; }
        Point parent {-1, -1};

        bool operator>(const Node& other) const {
            return f() > other.f();
        }
    };

    static std::vector<Point> neighbors(Point p);

    std::vector<Point> reconstructPath(
        const std::unordered_map<int, Node>& cameFrom, Point current) const;

    int hash(Point p) const { return p.y * 1024 + p.x; }
};

} // namespace core

#endif // CORE_AI_ASTARPATHFINDER_H
