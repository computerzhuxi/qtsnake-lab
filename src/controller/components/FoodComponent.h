#ifndef SNAKE_CONTROLLER_FOODCOMPONENT_H
#define SNAKE_CONTROLLER_FOODCOMPONENT_H

#include "GameState.h"
#include <random>

/// \brief 食物组件
/// \details 持有 mt19937 随机数引擎。
///          init() 首次生成食物，update() 检测蛇头是否吃到食物。
///          新食物从空闲集中随机选取并 erase，保证不和蛇身重叠。
class FoodComponent {
public:
    FoodComponent();

    void init(GameState& state);
    void update(GameState& state);

    /// \brief 更换随机种子（回放用，0=真随机）
    void setSeed(unsigned int seed);

private:
    /// \brief 从空闲集中随机选一个位置作为食物（erase 从空闲集移除）
    void spawnFood(GameState& state);

    std::mt19937 m_rng;  ///< Mersenne Twister 随机数引擎
};

#endif
