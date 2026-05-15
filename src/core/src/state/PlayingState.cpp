#include "core/state/PlayingState.h"
#include "core/engine/GameController.h"

namespace core {

void PlayingState::onEnter(GameController& controller) {
    controller.reset();
    controller.start();
}

void PlayingState::onExit(GameController& /*controller*/) {
    // Nothing to clean up.
}

void PlayingState::handleInput(GameController& controller, Direction d) {
    controller.setDirection(d);
}

void PlayingState::update(GameController& controller) {
    controller.tick();
}

} // namespace core
