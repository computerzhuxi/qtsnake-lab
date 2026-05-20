#ifndef SNAKE_CONTROLLER_MOVECOMPONENT_H
#define SNAKE_CONTROLLER_MOVECOMPONENT_H

#include "GameState.h"

/// \brief 移动组件
/// \details update(): 蛇移动 → 通过 Board::placeHead/removeTail 维护位掩码网格。
///          grid + freeCells 由 GameController::startGame() 通过 board.initFromSnakes() 构建。
class MoveComponent {
public:
    void update(GameState& state);
};

#endif
