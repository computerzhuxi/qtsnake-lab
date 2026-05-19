#ifndef SNAKE_CONTROLLER_ICOLLISIONDETECTOR_H
#define SNAKE_CONTROLLER_ICOLLISIONDETECTOR_H

#include <vector>
#include "CollisionReport.h"

class GameState;

/// \brief 碰撞检测器抽象接口
/// \details 纯函数：输入 const GameState&，输出 vector<CollisionReport>。
///          不修改 GameState，不持有状态，不产生副作用。
class ICollisionDetector {
public:
    virtual ~ICollisionDetector() = default;
    virtual std::vector<CollisionReport> detect(const GameState& state) const = 0;
};

#endif
