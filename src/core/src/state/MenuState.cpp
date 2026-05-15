#include "core/state/MenuState.h"
#include "core/engine/GameController.h"

namespace core {

void MenuState::onEnter(GameController& /*controller*/) {
    // Menu is idle — game not started yet.
}

void MenuState::onExit(GameController& /*controller*/) {
    // Nothing to clean up.
}

void MenuState::handleInput(GameController& /*controller*/, Direction /*d*/) {
    // Direction input is handled by StateMachine::startGame() instead.
    // Individual direction keys are not meaningful in menu.
}

void MenuState::update(GameController& /*controller*/) {
    // No game loop ticking in menu.
}

} // namespace core
