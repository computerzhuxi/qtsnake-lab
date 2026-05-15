#ifndef CORE_ENGINE_GAMECONFIG_H
#define CORE_ENGINE_GAMECONFIG_H

#include <cstdint>

namespace core {

struct GameConfig {
    int gridWidth = 30;
    int gridHeight = 20;
    int initialSpeed = 150;   // ms per tick
    int speedIncrement = 5;   // ms faster per food eaten
    int minSpeed = 50;        // fastest tick
    int initialSnakeLength = 3;
};

} // namespace core

#endif // CORE_ENGINE_GAMECONFIG_H
