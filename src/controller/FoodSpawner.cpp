#include "FoodSpawner.h"
#include "GameState.h"
#include "RngService.h"

FoodSpawner::FoodSpawner(RngService* rng)
    : m_rng(rng)
{
}

void FoodSpawner::init(GameState& state) {
    spawn(state);
}

void FoodSpawner::spawn(GameState& state) {
    auto& board = state.board;
    const auto& cells = board.freeCells();
    if (cells.empty()) return;

    int idx = m_rng->intInRange(0, static_cast<int>(cells.size()) - 1);
    auto it = cells.begin();
    std::advance(it, idx);
    Point foodPos = *it;
    board.removeFreeCell(foodPos);
    state.food.placeAt(foodPos, board);
    state.food.clearEaten();
}
