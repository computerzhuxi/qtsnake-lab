#include "CollisionComponent.h"
#include "GameState.h"

/// \brief 返回 mask 中最低置位的 index（0-7），mask 非零时调用
static int firstSetBit(uint8_t mask) {
    for (int i = 0; i < 8; ++i)
        if (mask & (1 << i))
            return i;
    return -1;
}

std::vector<CollisionReport> CollisionComponent::detect(const GameState& state) const {
    std::vector<CollisionReport> reports;
    const auto& board = state.board;

    for (size_t i = 0; i < state.snakes.size(); ++i) {
        Point head = state.snakes[i].head();
        int si = static_cast<int>(i);

        // 1. Wall
        if (board.isOutOfBounds(head)) {
            reports.push_back({si, CollisionType::Wall, head});
            continue;
        }

        const auto& cell = board.cellAt(head);
        uint8_t selfBit   = static_cast<uint8_t>(1 << si);
        uint8_t others     = static_cast<uint8_t>(cell.snakeMask & ~selfBit);
        uint8_t otherHeads = static_cast<uint8_t>(cell.headMask  & ~selfBit);

        // 2. HeadToHead / Body
        if (otherHeads) {
            reports.push_back({si, CollisionType::HeadToHead, head, firstSetBit(otherHeads)});
        } else if (others) {
            reports.push_back({si, CollisionType::OtherBody, head, firstSetBit(others)});
        }

        // 3. Food / Obstacle
        if (head == state.food.position()) {
            reports.push_back({si, CollisionType::Food, head});
        }
        if (cell.hasObstacle) {
            reports.push_back({si, CollisionType::Obstacle, head});
        }
    }

    return reports;
}
