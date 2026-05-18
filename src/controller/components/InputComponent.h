#ifndef SNAKE_CONTROLLER_INPUTCOMPONENT_H
#define SNAKE_CONTROLLER_INPUTCOMPONENT_H

#include "GameState.h"
#include "Direction.h"

/// \brief 键盘输入组件
/// \details 存储当前方向（默认 Right），update() 写入第一条蛇。
///          自带 180° 反转防护。setDirection() 由 AppShell 在键盘事件中调用。
class InputComponent {
public:
    /// \brief 从蛇同步初始方向（startGame 后调用）
    void init(const GameState& state);

    void update(GameState& state);
    void setDirection(Direction dir);

private:
    Direction m_direction = Direction::Right;
};

#endif
