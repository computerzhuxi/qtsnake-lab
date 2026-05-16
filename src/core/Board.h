#ifndef SNAKE_CORE_BOARD_H
#define SNAKE_CORE_BOARD_H

#include <random>
#include "Point.h"
#include "Snake.h"
#include "Food.h"

/// \brief 游戏棋盘：管理尺寸、食物生成、墙壁碰撞和食物碰撞检测
class Board {
public:
    Board(int width = 20, int height = 20, unsigned int seed = 0);
    int width() const;
    int height() const;
    Food food() const;
    void setSeed(unsigned int seed);
    void spawnFood(const Snake* snakeToAvoid = nullptr);
    bool checkWallCollision(const Snake& snake) const;
    bool checkFoodCollision(const Snake& snake) const;

private:
    int m_width;
    int m_height;
    Food m_food;
    std::mt19937 m_rng;
    std::uniform_int_distribution<int> m_distX;
    std::uniform_int_distribution<int> m_distY;
    Point randomPoint();
};

#endif // SNAKE_CORE_BOARD_H
