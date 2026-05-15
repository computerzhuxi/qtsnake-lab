#include "core/engine/Direction.h"

namespace core {

Direction opposite(Direction d) {
    switch (d) {
    case Direction::Up:    return Direction::Down;
    case Direction::Down:  return Direction::Up;
    case Direction::Left:  return Direction::Right;
    case Direction::Right: return Direction::Left;
    }
    return Direction::Up;
}

} // namespace core
