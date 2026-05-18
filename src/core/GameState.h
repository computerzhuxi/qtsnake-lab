#ifndef SNAKE_CORE_GAMESTATE_H
#define SNAKE_CORE_GAMESTATE_H

#include <vector>
#include "Board.h"
#include "Snake.h"

/// \brief 游戏状态快照（纯数据）
/// \details 聚合一帧的全部游戏数据：地图 + 所有蛇 + 游戏结束标志。
///          回放时可直接序列化，联机时即同步数据包。
///          Controller 是唯一的修改者。

struct GameState {
    Board board;
    std::vector<Snake> snakes;
    bool gameOver = false;
    int score = 0;
};

#endif // SNAKE_CORE_GAMESTATE_H
