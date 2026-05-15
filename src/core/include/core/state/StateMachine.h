#ifndef CORE_STATE_STATEMACHINE_H
#define CORE_STATE_STATEMACHINE_H

#include "core/state/GameState.h"
#include <memory>

namespace core {

class GameController;

class StateMachine {
public:
    explicit StateMachine(GameController& controller);

    void startGame();
    void togglePause();
    void returnToMenu();

    void handleInput(Direction d);
    void update();

    StateType currentType() const;

private:
    GameController& controller_;
    std::unique_ptr<GameState> currentState_;

    void changeState(std::unique_ptr<GameState> nextState);
};

} // namespace core

#endif // CORE_STATE_STATEMACHINE_H
