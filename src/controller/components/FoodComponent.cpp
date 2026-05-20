#include "FoodComponent.h"

FoodComponent::FoodComponent(RngService* rng)
    : m_rng(rng)
{
}

void FoodComponent::init(GameState& state) {
    spawnFood(state);
}

void FoodComponent::update(GameState& state) {
    if (state.gameOver) return;
    for (auto& s : state.snakes) {
        if (s.head() == state.board.foodPos()) {
            s.grow();
            state.score += state.board.foodPoints();
            spawnFood(state);
            return;
        }
    }
}

void FoodComponent::spawnFood(GameState& state) {
    auto& b = state.board;
    if (b.freeCells().empty()) return;
    auto it = b.freeCells().begin();
    int target = m_rng->intInRange(0, static_cast<int>(b.freeCells().size()) - 1);
    std::advance(it, target);
    Point food = *it;
    b.removeFreeCell(food);
    b.setFood(food);
}
