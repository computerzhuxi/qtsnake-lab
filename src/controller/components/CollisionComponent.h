#ifndef SNAKE_CONTROLLER_COLLISIONCOMPONENT_H
#define SNAKE_CONTROLLER_COLLISIONCOMPONENT_H

#include "GameState.h"

/// \brief 碰撞检测组件
/// \details 通过空闲集统一判断碰撞：
///          - head 在 freeCells 中 → 安全
///          - head == foodPos → 安全（吃食物，由 FoodComponent 处理）
///          - 其他 → gameOver（撞墙/撞自己/撞别的蛇）
class CollisionComponent {
public:
    void update(GameState& state);
};

#endif
