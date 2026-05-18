#include "InputComponent.h"

void InputComponent::init(const GameState& state) {
    if (!state.snakes.empty()) {
        m_direction = state.snakes[0].direction();
    }
}

void InputComponent::update(GameState& state) {
    if (!state.snakes.empty()) {
        state.snakes[0].setDirection(m_direction);
    }
}

void InputComponent::setDirection(Direction dir) {
    m_direction = dir;
}
