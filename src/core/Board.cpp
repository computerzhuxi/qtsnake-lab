#include "Board.h"

Board::Board(int w, int h)
    : m_width(w), m_height(h)
{
    ensureGrid(w, h);
}

void Board::setSize(int w, int h) {
    m_width = w;
    m_height = h;
    ensureGrid(w, h);
}

void Board::ensureGrid(int w, int h) {
    if (static_cast<int>(m_grid.size()) != h || (h > 0 && static_cast<int>(m_grid[0].size()) != w)) {
        m_grid.assign(h, std::vector<CellInfo>(w));
    }
}

void Board::initFromSnakes(const std::vector<Snake>& snakes) {
    ensureGrid(m_width, m_height);

    m_freeCells.clear();
    for (int y = 0; y < m_height; ++y)
        for (int x = 0; x < m_width; ++x)
            m_freeCells.insert({x, y});

    for (size_t i = 0; i < snakes.size(); ++i) {
        const auto& body = snakes[i].body();
        for (size_t k = 0; k < body.size(); ++k) {
            if (k == 0)
                placeHead(static_cast<int>(i), body[k]);
            else
                placeBody(static_cast<int>(i), body[k]);
        }
    }
}

void Board::placeHead(int snakeIdx, Point pos) {
    m_grid[pos.y][pos.x].snakeMask |= (1 << snakeIdx);
    m_grid[pos.y][pos.x].headMask  |= (1 << snakeIdx);
    m_freeCells.erase(pos);
}

void Board::placeBody(int snakeIdx, Point pos) {
    m_grid[pos.y][pos.x].snakeMask |= (1 << snakeIdx);
    m_freeCells.erase(pos);
}

void Board::removeHead(int snakeIdx, Point pos) {
    m_grid[pos.y][pos.x].headMask &= ~(1 << snakeIdx);
}

void Board::removeTail(int snakeIdx, Point pos) {
    m_grid[pos.y][pos.x].snakeMask &= ~(1 << snakeIdx);
    if (m_grid[pos.y][pos.x].snakeMask == 0)
        m_freeCells.insert(pos);
}

bool Board::isOutOfBounds(Point pos) const {
    return pos.x < 0 || pos.x >= m_width || pos.y < 0 || pos.y >= m_height;
}

const Board::CellInfo& Board::cellAt(Point pos) const {
    return m_grid[pos.y][pos.x];
}

void Board::setFood(Point pos) {
    m_foodPos = pos;
    m_grid[pos.y][pos.x].hasFood = true;
}

void Board::clearFood(Point pos) {
    m_grid[pos.y][pos.x].hasFood = false;
}

void Board::addObstacle(Point pos) {
    m_obstacles.insert(pos);
    m_grid[pos.y][pos.x].hasObstacle = true;
}
