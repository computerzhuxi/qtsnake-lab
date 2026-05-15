#include "core/engine/Food.h"
#include "core/engine/Grid.h"
#include "core/engine/Snake.h"
#include <algorithm>
#include <random>

namespace core {

Food::Food()
    : rng_(std::random_device{}())
{
}

void Food::setSeed(unsigned int seed) {
    rng_.seed(seed);
}

void Food::respawn(const Grid& grid, const Snake& snake) {
    std::vector<Point> freeCells;
    for (int y = 0; y < grid.height(); ++y) {
        for (int x = 0; x < grid.width(); ++x) {
            Point p{x, y};
            bool onSnake = false;
            for (const auto& seg : snake.body()) {
                if (seg == p) {
                    onSnake = true;
                    break;
                }
            }
            if (!onSnake) {
                freeCells.push_back(p);
            }
        }
    }

    if (!freeCells.empty()) {
        std::uniform_int_distribution<size_t> dist(0, freeCells.size() - 1);
        pos_ = freeCells[dist(rng_)];
        std::uniform_int_distribution<int> valDist(1, 3);
        value_ = valDist(rng_);
    }
}

} // namespace core
