#include "CollisionComponent.h"

void CollisionComponent::update(GameState& state) {
    auto& board = state.board;

    for (size_t i = 0; i < state.snakes.size(); ++i) {
        Point head = state.snakes[i].head();
        int si = static_cast<int>(i);

        // 1. Wall
        if (board.isOutOfBounds(head)) {
            state.gameOver = true;
            return;
        }

        const auto& cell = board.cellAt(head);
        uint8_t selfBit   = static_cast<uint8_t>(1 << si);
        uint8_t others     = static_cast<uint8_t>(cell.snakeMask & ~selfBit);
        uint8_t otherHeads = static_cast<uint8_t>(cell.headMask  & ~selfBit);

        // 2. HeadToHead
        if (otherHeads) {
            state.gameOver = true;
            return;
        }
        // 3. SelfBody / OtherBody
        if (others) {
            state.gameOver = true;
            return;
        }

        // 4. Food — 交由 FoodComponent 处理

        // 5. Obstacle
        if (cell.hasObstacle) {
            state.gameOver = true;
            return;
        }
    }
}
