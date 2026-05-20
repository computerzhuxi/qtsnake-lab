#include "MoveComponent.h"

void MoveComponent::update(GameState& state) {
    auto& board = state.board;
    for (size_t i = 0; i < state.snakes.size(); ++i) {
        Point oldHead = state.snakes[i].head();
        Point oldTail = state.snakes[i].tail();
        state.snakes[i].move();
        bool ate = state.snakes[i].growPending();  // 读即消费 m_growNext
        Point newHead = state.snakes[i].head();

        board.removeHead(static_cast<int>(i), oldHead);
        if (!ate)
            board.removeTail(static_cast<int>(i), oldTail);

        if (!board.isOutOfBounds(newHead))
            board.placeHead(static_cast<int>(i), newHead);
    }
}
