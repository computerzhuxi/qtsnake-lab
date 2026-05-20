#include "GridCollisionDetector.h"
#include "GameState.h"

GridCollisionDetector::GridCollisionDetector(int boardWidth, int boardHeight)
    : m_width(boardWidth), m_height(boardHeight)
{
    m_grid.assign(m_height, std::vector<CellInfo>(m_width));
    m_epochGrid.assign(m_height, std::vector<int>(m_width, 0));
}

void GridCollisionDetector::ensureSize(int width, int height) const {
    if (m_grid.empty() || static_cast<int>(m_grid.size()) != height
        || static_cast<int>(m_grid[0].size()) != width) {
        m_grid.assign(height, std::vector<CellInfo>(width));
        m_epochGrid.assign(height, std::vector<int>(width, 0));
        m_width = width;
        m_height = height;
    }
}

std::vector<CollisionReport> GridCollisionDetector::detect(const GameState& state) const {
    const auto& snakes = state.snakes;
    const auto& board = state.board;

    m_epoch++;
    ensureSize(board.width(), board.height());

    // === Phase 1: Build occupancy grid ===
    for (size_t i = 0; i < snakes.size(); ++i) {
        const auto& body = snakes[i].body();
        for (size_t k = 0; k < body.size(); ++k) {
            Point p = body[k];
            if (p.x < 0 || p.x >= board.width() || p.y < 0 || p.y >= board.height())
                continue;
            if (m_epochGrid[p.y][p.x] != m_epoch) {
                m_grid[p.y][p.x] = CellInfo{};
                m_epochGrid[p.y][p.x] = m_epoch;
            }
            CellInfo& cell = m_grid[p.y][p.x];
            cell.totalSegs++;
            if (k == 0) {
                if (cell.headCount == 0)
                    cell.firstHeadIdx = static_cast<int>(i);
                else if (cell.headCount == 1)
                    cell.secondHeadIdx = static_cast<int>(i);
                cell.headCount++;
            } else {
                cell.bodySnakeIdx = static_cast<int>(i);
            }
        }
    }

    // === Phase 2: Detect collisions ===
    std::vector<CollisionReport> reports;
    for (size_t i = 0; i < snakes.size(); ++i) {
        Point head = snakes[i].head();

        // 1. Wall
        if (head.x < 0 || head.x >= board.width() ||
            head.y < 0 || head.y >= board.height()) {
            reports.push_back({static_cast<int>(i), CollisionType::Wall, head});
            continue;
        }

        CellInfo& cell = m_grid[head.y][head.x];

        // 2. HeadToHead
        if (cell.headCount >= 2) {
            int other = (cell.firstHeadIdx == static_cast<int>(i))
                            ? cell.secondHeadIdx
                            : cell.firstHeadIdx;
            reports.push_back({static_cast<int>(i), CollisionType::HeadToHead, head, other});
            continue;
        }

        // 3. SelfBody / OtherBody
        if (cell.totalSegs > cell.headCount) {
            if (cell.bodySnakeIdx == static_cast<int>(i)) {
                reports.push_back({static_cast<int>(i), CollisionType::SelfBody, head});
            } else if (cell.bodySnakeIdx >= 0) {
                reports.push_back({static_cast<int>(i), CollisionType::OtherBody, head, cell.bodySnakeIdx});
            }
            continue;
        }

        // 4. Food
        if (head == board.foodPos()) {
            reports.push_back({static_cast<int>(i), CollisionType::Food, head});
        }

        // 5. Obstacle
        if (board.obstacles().count(head)) {
            reports.push_back({static_cast<int>(i), CollisionType::Obstacle, head});
        }
    }

    return reports;
}
