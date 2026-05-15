#include "core/state/PausedState.h"
#include "core/engine/GameController.h"

namespace core {

void PausedState::onEnter(GameController& controller) {
    controller.pause();
}

void PausedState::onExit(GameController& /*controller*/) {
    // Nothing to clean up.
}

void PausedState::handleInput(GameController& /*controller*/, Direction /*d*/) {
    // Input is ignored while paused.
    // Resuming is handled by StateMachine::togglePause().
}

void PausedState::update(GameController& /*controller*/) {
    // No-op: game is paused, no ticking.
}

} // namespace core
