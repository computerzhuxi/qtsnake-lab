#ifndef SNAKE_CONTROLLER_FOODSPAWNER_H
#define SNAKE_CONTROLLER_FOODSPAWNER_H

class RngService;
struct GameState;

/// \brief 食物生成器：从 freeCells 随机选取一格生成食物
/// \details 不含碰撞检测——检测与响应已在 CollisionComponent + processCollisions 中完成。
class FoodSpawner {
public:
    explicit FoodSpawner(RngService* rng);
    void init(GameState& state);
    void spawn(GameState& state);
private:
    RngService* m_rng;
};

#endif
