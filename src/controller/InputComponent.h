#ifndef SNAKE_CONTROLLER_INPUTCOMPONENT_H
#define SNAKE_CONTROLLER_INPUTCOMPONENT_H

#include "GameState.h"
#include "Direction.h"

/// \brief 键盘输入组件
/// \details 仅缓存最新方向输入（默认 Right）；update() 将缓存写入第一条蛇。
///          180° 反转防护由 Snake::setDirection 在 update() 阶段统一执行，
///          本组件不做任何方向校验。
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
