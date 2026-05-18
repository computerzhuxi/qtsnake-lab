#include "MoveComponent.h"

void MoveComponent::init(GameState& state) {
    auto& b = state.board;
    b.freeCells.clear();
    for (int y = 0; y < b.height; ++y)
        for (int x = 0; x < b.width; ++x)
            b.freeCells.insert({x, y});
    for (auto& s : state.snakes)
        for (auto& seg : s.body())
            b.freeCells.erase(seg);
}

void MoveComponent::update(GameState& state) {
    auto& free = state.board.freeCells;
    for (auto& s : state.snakes) {
        Point tail = s.body().back();
        bool grew = s.growPending();
        s.move();
        if (!grew) {
            free.insert(tail);  // 旧尾释放
        }
        // 新头不移除——留给 CollisionComponent 处理
    }
}
