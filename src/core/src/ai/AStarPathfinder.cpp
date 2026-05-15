#include "core/ai/AStarPathfinder.h"
#include "core/engine/Grid.h"
#include "core/engine/Snake.h"
#include <queue>
#include <unordered_map>

namespace core {

int AStarPathfinder::manhattan(Point a, Point b) {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

std::vector<Point> AStarPathfinder::neighbors(Point p) {
    return {
        {p.x, p.y - 1}, // Up
        {p.x, p.y + 1}, // Down
        {p.x - 1, p.y}, // Left
        {p.x + 1, p.y}, // Right
    };
}

PathResult AStarPathfinder::findPath(const Grid& grid, const Snake& snake,
                                      Point target) {
    PathResult result;
    Point start = snake.head();

    if (start == target) {
        result.found = true;
        result.path = {start};
        return result;
    }

    // Collect obstacle points: snake body EXCEPT the tail
    // (the tail moves away each step, so it's safe to step into)
    std::unordered_set<int> blocked;
    const auto& body = snake.body();
    for (size_t i = 0; i < body.size(); ++i) {
        if (i == body.size() - 1 && body.size() > 1) continue;
        blocked.insert(hash(body[i]));
    }

    // Also block cells outside the grid
    for (int y = -1; y <= grid.height(); ++y) {
        blocked.insert(hash({-1, y}));
        blocked.insert(hash({grid.width(), y}));
    }
    for (int x = -1; x <= grid.width(); ++x) {
        blocked.insert(hash({x, -1}));
        blocked.insert(hash({x, grid.height()}));
    }

    // A* search
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openSet;
    std::unordered_map<int, Node> cameFrom;
    std::unordered_set<int> closed;

    Node startNode {start, 0, manhattan(start, target), {-1, -1}};
    openSet.push(startNode);
    cameFrom[hash(start)] = startNode;

    while (!openSet.empty()) {
        Node current = openSet.top();
        openSet.pop();

        int h = hash(current.pos);
        if (closed.count(h)) continue;
        closed.insert(h);

        if (current.pos == target) {
            result.found = true;
            result.cost = current.g;
            result.path = reconstructPath(cameFrom, current.pos);
            return result;
        }

        for (Point nextPos : neighbors(current.pos)) {
            int nh = hash(nextPos);
            if (blocked.count(nh) || closed.count(nh)) continue;

            int newG = current.g + 1;
            auto it = cameFrom.find(nh);
            if (it == cameFrom.end() || newG < it->second.g) {
                Node next {nextPos, newG, manhattan(nextPos, target), current.pos};
                cameFrom[nh] = next;
                openSet.push(next);
            }
        }
    }

    // No path found
    result.found = false;
    return result;
}

std::vector<Point> AStarPathfinder::reconstructPath(
    const std::unordered_map<int, Node>& cameFrom, Point current) const
{
    std::vector<Point> path;
    int h = hash(current);
    auto it = cameFrom.find(h);
    while (it != cameFrom.end()) {
        path.push_back(it->second.pos);
        Point parent = it->second.parent;
        if (parent.x < 0 && parent.y < 0) break;
        it = cameFrom.find(hash(parent));
    }
    std::reverse(path.begin(), path.end());
    return path;
}

} // namespace core
