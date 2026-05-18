#ifndef SNAKE_CONTROLLER_MOVECOMPONENT_H
#define SNAKE_CONTROLLER_MOVECOMPONENT_H

#include "GameState.h"

/// \brief 移动组件
/// \details update(): 蛇移动 → 旧尾 insert 回空闲集（释放）。
///          新头不 erase——留给 CollisionComponent 在碰撞通过后处理。
///          init() 首次构建完整空闲格集。
class MoveComponent {
public:
    void init(GameState& state);
    void update(GameState& state);
};

#endif
