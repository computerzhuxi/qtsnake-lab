# T-15：CollisionComponent 纯 detect + Controller 处理响应

> 签发：主程 | 日期：2026-05-19 | 状态：待领取
> 分支：`phase-3-collision-refactor`

---

## 任务描述

1. **CollisionComponent** 从"检测+响应一体"改为纯检测——`detect(state)` 返回 `vector<CollisionReport>`，不设 `gameOver`，不修改状态。
2. **Controller** 新增 `processCollisions(reports)` 私有方法处理碰撞响应（死亡/食物/gameOver）。
3. `update()` 调 detect 后调 processCollisions，职责分离。

---

## 输入约束

### 允许修改的文件

```
~ src/controller/components/CollisionComponent.h   (接口改为 detect)
~ src/controller/components/CollisionComponent.cpp (位掩码检测，删除响应逻辑)
~ src/controller/GameController.h                   (新增 processCollisions 声明)
~ src/controller/GameController.cpp                 (调 detect + processCollisions)
~ tests/test_components.cpp                         (测试适配)
```

### 不允许做的事

- 不得修改 Board / Snake / MoveComponent / FoodComponent
- 不得修改 `CollisionReport.h`、`GameState.h`
- 不得改 CMakeLists.txt
- 不得删除任何文件（T-16 统一清理）

---

## 输出约束

### 1. CollisionComponent

```cpp
class CollisionComponent {
public:
    /// \brief 纯检测——读 Board::grid 的位掩码，返回碰撞报告列表。
    /// \details 不修改 GameState，不产生副作用。
    std::vector<CollisionReport> detect(const GameState& state) const;
};
```

检测算法：

```
for each snake i:
  head = snakes[i].head()

  // Wall
  if board.isOutOfBounds(head):
    reports += {i, Wall, head}; continue

  cell = board.cellAt(head)
  others     = cell.snakeMask & ~(1 << i)
  otherHeads = cell.headMask  & ~(1 << i)

  // HeadToHead / Body
  if otherHeads:
    reports += {i, HeadToHead, head, firstSetBit(otherHeads)}
  else if others:
    reports += {i, OtherBody, head, firstSetBit(others)}

  // Food / Obstacle
  if cell.hasFood:
    reports += {i, Food, head}
  if cell.hasObstacle:
    reports += {i, Obstacle, head}
```

- `firstSetBit(uint8_t mask)`：内联辅助函数，返回最低置位 index（0-7），mask 非零时调用

### 2. Controller

```cpp
// GameController.h
private:
    void processCollisions(const std::vector<CollisionReport>& reports);
```

```cpp
// GameController.cpp
void GameController::update() {
    if (m_phase != State::Playing) return;

    m_render->update(m_state);
    m_input->update(m_state);
    m_move->update(m_state);

    auto reports = m_collision->detect(m_state);
    processCollisions(reports);

    m_food->update(m_state);

    if (m_state.gameOver) {
        m_timer->stop();
        m_phase = State::GameOver;
        emit stateChanged(m_phase);
        emit scoreChanged(m_state.score);
        LOG_INFO("GameController", "State: Playing -> GameOver, score=" + std::to_string(m_state.score));
    }
}

void GameController::processCollisions(const std::vector<CollisionReport>& reports) {
    bool anyoneAte = false;

    for (auto& r : reports) {
        auto& snake = m_state.snakes[r.snakeIndex];

        switch (r.type) {
        case CollisionType::Wall:
        case CollisionType::SelfBody:
        case CollisionType::OtherBody:
        case CollisionType::Obstacle:
            snake.alive = false;
            break;
        case CollisionType::HeadToHead:
            snake.alive = false;
            if (r.otherSnakeIndex >= 0)
                m_state.snakes[r.otherSnakeIndex].alive = false;
            break;
        case CollisionType::Food:
            if (snake.alive) {
                snake.grow();
                anyoneAte = true;
            }
            break;
        }
    }

    // gameOver 判定：所有蛇都死了
    bool allDead = true;
    for (auto& s : m_state.snakes)
        if (s.alive) { allDead = false; break; }
    if (allDead) m_state.gameOver = true;

    if (anyoneAte)
        m_state.board.markFoodEaten();
}
```

流程：

```
1. Render     → syncFromState(state)
2. Input      → applyDirections(state)
3. Move       → moveAll(state) + 维护 grid
4. Collision  → detect(state) → reports
5. processCollisions(reports)  → 死亡/食物/gameOver
6. Food       → update(state)（T-16 改 FoodSpawner）
```

---

## 验收标准

1. 编译 /W4 零警告
2. 63 个既有测试全部适配通过
3. `CollisionComponent::detect()` const，不修改参数
4. 位掩码检测与旧判定一致
5. Controller::update() 清晰——每步一行
6. 手工冒烟：单人→吃食物→撞墙→再来一局正常

---

## 提交格式

```
改动文件清单:
~ src/controller/components/CollisionComponent.h
~ src/controller/components/CollisionComponent.cpp
~ src/controller/GameController.h
~ src/controller/GameController.cpp
~ tests/test_components.cpp

新增/修改测试:
(既有 CollisionComponent 测试适配为调 detect + 手动验证 reports)

一句话冒烟:
编译 /W4 零警告，63 测试全绿，SnakeArena.exe 单人正常。
```
