#ifndef CORE_STATE_PAUSEDSTATE_H
#define CORE_STATE_PAUSEDSTATE_H

#include "core/state/GameState.h"

namespace core {

class PausedState : public GameState {
public:
    void onEnter(GameController& controller) override;
    void onExit(GameController& controller) override;
    void handleInput(GameController& controller, Direction d) override;
    void update(GameController& controller) override;
    StateType type() const override { return StateType::Paused; }
};

} // namespace core

#endif // CORE_STATE_PAUSEDSTATE_H
