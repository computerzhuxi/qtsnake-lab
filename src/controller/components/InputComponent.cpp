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
    if ((m_direction == Direction::Up    && dir == Direction::Down)  ||
        (m_direction == Direction::Down  && dir == Direction::Up)    ||
        (m_direction == Direction::Left  && dir == Direction::Right) ||
        (m_direction == Direction::Right && dir == Direction::Left))
        return;
    m_direction = dir;
}
