#ifndef SNAKE_CORE_COLLISIONREPORT_H
#define SNAKE_CORE_COLLISIONREPORT_H

#include "Point.h"

/// \brief 碰撞类型
enum class CollisionType {
    None,
    Wall,         // head 出界
    SelfBody,     // head 撞自己身体
    OtherBody,    // head 撞另一条蛇的身体
    HeadToHead,   // 两条蛇的 head 在同一格
    Food,         // head 在食物格上
    Obstacle,     // head 在障碍物格上（预留）
};

/// \brief 一次碰撞事件（纯数据）
struct CollisionReport {
    int snakeIndex;              // 哪条蛇发生了碰撞
    CollisionType type;          // 撞了什么
    Point position;              // 碰撞发生的格子坐标
    int otherSnakeIndex = -1;    // 如果涉及另一条蛇，它的索引（HeadToHead / OtherBody）
};

#endif
