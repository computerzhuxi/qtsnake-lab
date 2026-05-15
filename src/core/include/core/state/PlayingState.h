#ifndef CORE_STATE_PLAYINGSTATE_H
#define CORE_STATE_PLAYINGSTATE_H

#include "core/state/GameState.h"

namespace core {

class PlayingState : public GameState {
public:
    void onEnter(GameController& controller) override;
    void onExit(GameController& controller) override;
    void handleInput(GameController& controller, Direction d) override;
    void update(GameController& controller) override;
    StateType type() const override { return StateType::Playing; }
};

} // namespace core

#endif // CORE_STATE_PLAYINGSTATE_H
