#ifndef SNAKE_CONTROLLER_FOODCOMPONENT_H
#define SNAKE_CONTROLLER_FOODCOMPONENT_H

#include "GameState.h"
#include "RngService.h"

/// \brief 食物组件
/// \details 持有 RngService 指针（由外部 GameController 管理生命周期）。
///          init() 首次生成食物，update() 检测蛇头是否吃到食物。
///          新食物从空闲集中随机选取并 erase，保证不和蛇身重叠。
class FoodComponent {
public:
    explicit FoodComponent(RngService* rng);

    void init(GameState& state);
    void update(GameState& state);

private:
    /// \brief 从空闲集中随机选一个位置作为食物（erase 从空闲集移除）
    void spawnFood(GameState& state);

    RngService* m_rng;  ///< 随机数服务（非拥有指针）
};

#endif
