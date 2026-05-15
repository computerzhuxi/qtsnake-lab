#ifndef CORE_STATE_GAMESTATE_H
#define CORE_STATE_GAMESTATE_H

#include "core/engine/Direction.h"

namespace core {

class GameController;

enum class StateType {
    Menu,
    Playing,
    Paused,
    GameOver
};

class GameState {
public:
    virtual ~GameState() = default;

    virtual void onEnter(GameController& controller) = 0;
    virtual void onExit(GameController& controller) = 0;
    virtual void handleInput(GameController& controller, Direction d) = 0;
    virtual void update(GameController& controller) = 0;
    virtual StateType type() const = 0;
};

} // namespace core

#endif // CORE_STATE_GAMESTATE_H
