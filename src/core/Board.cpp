#include "Board.h"
#include <algorithm>

Board::Board(int width, int height, unsigned int seed)
    : m_width(width)
    , m_height(height)
    , m_rng(seed == 0 ? std::random_device{}() : seed)
    , m_distX(0, width - 1)
    , m_distY(0, height - 1)
{
}

int Board::width() const { return m_width; }
int Board::height() const { return m_height; }
Food Board::food() const { return m_food; }

void Board::setSeed(unsigned int seed) {
    m_rng.seed(seed == 0 ? std::random_device{}() : seed);
}

void Board::spawnFood(const Snake* snakeToAvoid) {
    Point pos = randomPoint();
    if (snakeToAvoid) {
        int maxAttempts = 100;
        int attempt = 0;
        while (attempt < maxAttempts) {
            bool onSnake = false;
            for (const auto& seg : snakeToAvoid->body()) {
                if (pos == seg) { onSnake = true; break; }
            }
            if (!onSnake) break;
            pos = randomPoint();
            ++attempt;
        }
    }
    m_food.position = pos;
}

Point Board::randomPoint() {
    return {m_distX(m_rng), m_distY(m_rng)};
}

bool Board::checkWallCollision(const Snake& snake) const {
    Point h = snake.head();
    return h.x < 0 || h.x >= m_width || h.y < 0 || h.y >= m_height;
}

bool Board::checkFoodCollision(const Snake& snake) const {
    return snake.head() == m_food.position;
}
