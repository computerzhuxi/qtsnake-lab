#ifndef SNAKE_CORE_GAMESTATE_H
#define SNAKE_CORE_GAMESTATE_H

#include <vector>
#include "Board.h"
#include "Snake.h"
#include "Food.h"

/// \brief 游戏状态快照（纯数据）
/// \details 聚合一帧的全部游戏数据：地图 + 所有蛇 + 食物 + 游戏结束标志。
///          Controller 是唯一的修改者。

struct GameState {
    Board board;
    std::vector<Snake> snakes;
    Food food;
    int score = 0;
    bool gameOver = false;
};

#endif // SNAKE_CORE_GAMESTATE_H
