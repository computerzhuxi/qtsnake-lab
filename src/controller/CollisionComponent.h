#ifndef SNAKE_CONTROLLER_COLLISIONCOMPONENT_H
#define SNAKE_CONTROLLER_COLLISIONCOMPONENT_H

#include <vector>
#include "CollisionReport.h"

struct GameState;

/// \brief 碰撞检测组件（纯检测，不修改状态）
/// \details 读 Board::grid 位掩码，返回 CollisionReport 列表。
///          不修改 GameState，不产生副作用。
class CollisionComponent {
public:
    std::vector<CollisionReport> detect(const GameState& state) const;
};

#endif
