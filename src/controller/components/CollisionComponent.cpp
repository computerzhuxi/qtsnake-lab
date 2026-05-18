#include "CollisionComponent.h"

void CollisionComponent::update(GameState& state) {
    auto& board = state.board;

    for (size_t i = 0; i < state.snakes.size(); ++i) {
        Point h = state.snakes[i].head();

        if (board.freeCells.count(h) == 0 && h != board.foodPos) {
            state.gameOver = true;
            return;
        }

        for (size_t j = i + 1; j < state.snakes.size(); ++j) {
            if (h == state.snakes[j].head()) {
                state.gameOver = true;
                return;
            }
        }
    }

    for (auto& s : state.snakes) {
        if (s.head() != board.foodPos) {
            board.freeCells.erase(s.head());
        }
    }
}
