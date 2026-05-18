#ifndef SNAKE_CORE_BOARD_H
#define SNAKE_CORE_BOARD_H

#include <unordered_set>
#include "Point.h"

/// \brief 棋盘地图数据（纯数据，无逻辑）
/// \details 存储地图尺寸、空闲格子集、食物位置。
///          空闲格子集由 Controller 维护，食物位置由 Controller 更新。
struct Board {
    int width = 20;
    int height = 20;
    std::unordered_set<Point> freeCells;  ///< 当前空闲的格子
    Point foodPos;                        ///< 食物位置
    int foodPoints = 1;                   ///< 当前食物分值
};

#endif // SNAKE_CORE_BOARD_H
