#ifndef CORE_STATE_GAMEOVERSTATE_H
#define CORE_STATE_GAMEOVERSTATE_H

#include "core/state/GameState.h"

namespace core {

class GameOverState : public GameState {
public:
    void onEnter(GameController& controller) override;
    void onExit(GameController& controller) override;
    void handleInput(GameController& controller, Direction d) override;
    void update(GameController& controller) override;
    StateType type() const override { return StateType::GameOver; }
};

} // namespace core

#endif // CORE_STATE_GAMEOVERSTATE_H
