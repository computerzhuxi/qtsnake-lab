#include "app/ai/AIWorker.h"
#include "core/ai/AStarPathfinder.h"
#include "core/engine/Direction.h"
#include "core/engine/Grid.h"
#include "core/engine/Snake.h"

AIWorker::AIWorker(QObject* parent)
    : QObject(parent)
{
}

void AIWorker::setPathfinder(std::unique_ptr<core::IPathfinder> pathfinder) {
    pathfinder_ = std::move(pathfinder);
}

void AIWorker::computeNextMove(QPoint qhead, QPoint qfood,
                                QVector<QPoint> qbody,
                                int /*currentDir*/,
                                int gridWidth, int gridHeight)
{
    if (!pathfinder_) return;

    core::Point head {qhead.x(), qhead.y()};
    core::Point food {qfood.x(), qfood.y()};

    core::Grid grid(gridWidth, gridHeight);

    // Reconstruct snake body
    std::deque<core::Point> body;
    body.push_back(head);
    for (const auto& qp : qbody) {
        body.push_back({qp.x(), qp.y()});
    }
    core::Snake snake(head, 1, core::Direction::Right);
    snake.setBody(body, core::Direction::Right);

    core::PathResult result = pathfinder_->findPath(grid, snake, food);

    if (result.found && result.path.size() >= 2) {
        core::Point next = result.path[1];
        if (next.x == head.x && next.y == head.y - 1)
            emit moveComputed(core::Direction::Up);
        else if (next.x == head.x && next.y == head.y + 1)
            emit moveComputed(core::Direction::Down);
        else if (next.x == head.x - 1 && next.y == head.y)
            emit moveComputed(core::Direction::Left);
        else
            emit moveComputed(core::Direction::Right);
    } else {
        // Survival: try safe directions
        std::vector<core::Point> dirs = {
            {head.x, head.y - 1}, // Up
            {head.x, head.y + 1}, // Down
            {head.x - 1, head.y}, // Left
            {head.x + 1, head.y}, // Right
        };
        std::vector<core::Direction> moveDirs = {
            core::Direction::Up, core::Direction::Down,
            core::Direction::Left, core::Direction::Right
        };

        for (size_t i = 0; i < dirs.size(); ++i) {
            core::Point p = dirs[i];
            if (!grid.isInside(p)) continue;
            bool onBody = false;
            for (size_t j = 0; j + 1 < body.size(); ++j) {
                if (body[j] == p) { onBody = true; break; }
            }
            if (!onBody) {
                emit moveComputed(moveDirs[i]);
                return;
            }
        }
        emit moveComputed(core::Direction::Right); // doomed
    }
}
