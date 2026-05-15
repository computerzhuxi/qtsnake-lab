#include "core/state/StateMachine.h"
#include "core/state/MenuState.h"
#include "core/state/PlayingState.h"
#include "core/state/PausedState.h"
#include "core/state/GameOverState.h"
#include "core/engine/GameController.h"

namespace core {

StateMachine::StateMachine(GameController& controller)
    : controller_(controller)
    , currentState_(std::make_unique<MenuState>())
{
}

void StateMachine::changeState(std::unique_ptr<GameState> nextState) {
    if (currentState_) {
        currentState_->onExit(controller_);
    }
    currentState_ = std::move(nextState);
    currentState_->onEnter(controller_);
}

void StateMachine::startGame() {
    if (currentType() == StateType::Menu || currentType() == StateType::GameOver) {
        changeState(std::make_unique<PlayingState>());
    }
}

void StateMachine::togglePause() {
    if (currentType() == StateType::Playing) {
        changeState(std::make_unique<PausedState>());
    } else if (currentType() == StateType::Paused) {
        changeState(std::make_unique<PlayingState>());
    }
}

void StateMachine::returnToMenu() {
    changeState(std::make_unique<MenuState>());
}

void StateMachine::handleInput(Direction d) {
    if (currentState_) {
        currentState_->handleInput(controller_, d);
    }
}

void StateMachine::update() {
    if (currentState_) {
        currentState_->update(controller_);
    }

    if (currentType() == StateType::Playing && controller_.isGameOver()) {
        changeState(std::make_unique<GameOverState>());
    }
}

StateType StateMachine::currentType() const {
    return currentState_ ? currentState_->type() : StateType::Menu;
}

} // namespace core
