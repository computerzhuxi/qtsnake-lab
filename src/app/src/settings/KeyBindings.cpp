#include "app/settings/KeyBindings.h"
#include <QKeySequence>

core::Direction KeyBindings::keyToDirection(int key) const {
    if (key == keyUp)    return core::Direction::Up;
    if (key == keyDown)  return core::Direction::Down;
    if (key == keyLeft)  return core::Direction::Left;
    if (key == keyRight) return core::Direction::Right;
    return core::Direction::Right; // fallback
}

int KeyBindings::directionToKey(core::Direction d) const {
    switch (d) {
    case core::Direction::Up:    return keyUp;
    case core::Direction::Down:  return keyDown;
    case core::Direction::Left:  return keyLeft;
    case core::Direction::Right: return keyRight;
    }
    return keyRight;
}

std::map<int, QString> KeyBindings::toMap() const {
    return {
        {keyUp,    "Up"},
        {keyDown,  "Down"},
        {keyLeft,  "Left"},
        {keyRight, "Right"},
        {keyPause, "Pause"},
        {keyConfirm, "Confirm"},
        {keyBack,  "Back"}
    };
}

KeyBindings KeyBindings::defaults() {
    return KeyBindings{};
}
