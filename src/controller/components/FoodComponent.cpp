#include "FoodComponent.h"

FoodComponent::FoodComponent() {
    std::random_device rd;
    m_rng.seed(rd());
}

void FoodComponent::init(GameState& state) {
    spawnFood(state);
}

void FoodComponent::update(GameState& state) {
    if (state.gameOver) return;
    for (auto& s : state.snakes) {
        if (s.head() == state.board.foodPos) {
            s.grow();
            state.score += state.board.foodPoints;
            spawnFood(state);
            return;
        }
    }
}

void FoodComponent::setSeed(unsigned int seed) {
    m_rng.seed(seed == 0 ? std::random_device{}() : seed);
}

void FoodComponent::spawnFood(GameState& state) {
    auto& b = state.board;
    if (b.freeCells.empty()) return;
    auto it = b.freeCells.begin();
    int target = std::uniform_int_distribution<int>(0, (int)b.freeCells.size() - 1)(m_rng);
    std::advance(it, target);
    b.foodPos = *it;
    b.freeCells.erase(it);
}
