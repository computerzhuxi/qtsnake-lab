#ifndef CORE_ENGINE_DIRECTION_H
#define CORE_ENGINE_DIRECTION_H

namespace core {

enum class Direction {
    Up,
    Down,
    Left,
    Right
};

Direction opposite(Direction d);

} // namespace core

#endif // CORE_ENGINE_DIRECTION_H
