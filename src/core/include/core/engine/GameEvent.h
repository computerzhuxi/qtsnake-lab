#ifndef CORE_ENGINE_GAMEEVENT_H
#define CORE_ENGINE_GAMEEVENT_H

#include "Direction.h"
#include "Point.h"
#include <cstdint>

namespace core {

enum class GameEventType {
    Tick,
    Input,
    FoodEaten,
    FoodSpawned,
    Collision,
    StateChange,
    GameOver
};

struct GameEvent {
    GameEventType type;
    Direction inputDirection = Direction::Up;
    Point foodPosition;
    int score = 0;
    uint64_t tickNumber = 0;
};

} // namespace core

#endif // CORE_ENGINE_GAMEEVENT_H
