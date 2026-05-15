#include "core/state/GameOverState.h"
#include "core/engine/GameController.h"

namespace core {

void GameOverState::onEnter(GameController& /*controller*/) {
    // Game is already over — collision detection already happened.
    // The final score is preserved in the controller for display.
}

void GameOverState::onExit(GameController& /*controller*/) {
    // Nothing to clean up.
}

void GameOverState::handleInput(GameController& /*controller*/, Direction /*d*/) {
    // Input is ignored on game over screen.
    // Returning to menu is handled by StateMachine::returnToMenu().
}

void GameOverState::update(GameController& /*controller*/) {
    // No more ticks — game is over.
}

} // namespace core
