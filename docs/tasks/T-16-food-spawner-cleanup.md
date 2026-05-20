# T-16：FoodSpawner + 删旧文件 + Controller 流水线收尾

> 签发：主程 | 日期：2026-05-19 | 状态：待领取
> 分支：`phase-3-collision-refactor`

---

## 任务描述

碰撞线最后一张卡——收尾清理：

1. **FoodComponent → FoodSpawner**：重命名，删除吃食物检测逻辑（已在 T-15 由 CollisionComponent + processCollisions 接管），只保留从 freeCells 随机生成食物。
2. **删除旧文件**：`ICollisionDetector.h`、`NaiveCollisionDetector.h/.cpp`、`GridCollisionDetector.h/.cpp`。
3. **Controller / CMakeLists 适配**。

---

## 输入约束

### 允许修改/删除的文件

```
重命名: src/controller/components/FoodComponent.h  → src/controller/FoodSpawner.h
重命名: src/controller/components/FoodComponent.cpp → src/controller/FoodSpawner.cpp

删除:   src/controller/ICollisionDetector.h
删除:   src/controller/NaiveCollisionDetector.h
删除:   src/controller/NaiveCollisionDetector.cpp
删除:   src/controller/GridCollisionDetector.h
删除:   src/controller/GridCollisionDetector.cpp

修改:   src/controller/CMakeLists.txt       (删旧，加 FoodSpawner)
修改:   tests/CMakeLists.txt                (删 Naive/Grid 测试源文件)
修改:   src/controller/GameController.h     (FoodComponent → FoodSpawner)
修改:   src/controller/GameController.cpp   (同上)
修改:   tests/test_components.cpp           (FoodComponent 测试适配 FoodSpawner)
删除:   tests/test_collision_detector.cpp   (依赖 NaiveDetector/GridDetector)
```

### 不允许做的事

- 不得修改 Board / Snake / MoveComponent / CollisionComponent / RenderComponent / InputComponent
- 不得修改 `CollisionReport.h`、`GameState.h`
- 不得引入新依赖

---

## 输出约束

### 1. FoodSpawner

```cpp
// src/controller/FoodSpawner.h
class RngService;
struct GameState;

class FoodSpawner {
public:
    explicit FoodSpawner(RngService* rng);
    void init(GameState& state);
    void spawn(GameState& state);   // 从 freeCells 随机生成新食物
private:
    RngService* m_rng;
};
```

```cpp
// FoodSpawner.cpp
void FoodSpawner::init(GameState& state) {
    // 从 freeCells 随机选一格
    spawn(state);
}

void FoodSpawner::spawn(GameState& state) {
    auto& board = state.board;
    const auto& cells = board.freeCells();
    if (cells.empty()) return;

    int idx = m_rng->intInRange(0, static_cast<int>(cells.size()) - 1);
    auto it = cells.begin();
    std::advance(it, idx);
    board.setFood(*it);
    board.clearFoodEaten();
}
```

**关键变化**：不再检查 `head == foodPos`——这个检测已在 `CollisionComponent::detect()` 中完成，响应已在 `processCollisions()` 中完成。`FoodSpawner::spawn()` 只在 Controller 判断 `board.isFoodEaten()` 后调用。

### 2. Controller 适配

```cpp
// GameController.h
#include "FoodSpawner.h"
std::unique_ptr<FoodSpawner> m_food;  // 替代 FoodComponent

// GameController.cpp
GameController::GameController(...)
    : m_food(std::make_unique<FoodSpawner>(m_rng.get()))  // 同前

void GameController::update() {
    // ... render/input/move/detect/processCollisions ...

    if (m_state.board.isFoodEaten())
        m_food->spawn(m_state);
    // 不再无条件调 food->update()
}
```

### 3. CMakeLists.txt

```
src/controller/CMakeLists.txt:
  移除: components/FoodComponent.cpp, NaiveCollisionDetector.cpp, GridCollisionDetector.cpp
  追加: FoodSpawner.cpp

tests/CMakeLists.txt:
  移除: test_collision_detector.cpp
```

### 4. 删除文件清单

```
src/controller/ICollisionDetector.h
src/controller/NaiveCollisionDetector.h
src/controller/NaiveCollisionDetector.cpp
src/controller/GridCollisionDetector.h
src/controller/GridCollisionDetector.cpp
tests/test_collision_detector.cpp
```

### 5. 最终流水线

```
Controller::update():
  1. Render        → syncFromState(state)
  2. Input         → applyDirections(state)
  3. Move          → moveAll(state) + 维护 grid
  4. Collision     → detect(state) → reports
  5. processCollisions(reports)     → 死亡/食物/gameOver
  6. FoodSpawner   → if isFoodEaten(): spawn(state)
```

---

## 验收标准

1. 编译 /W4 零警告
2. 删掉 6 个文件后无未解析符号
3. 既有测试全部适配通过（test_collision_detector.cpp 删除后测试数减少 17 个 → 46 test）
4. `FoodSpawner::spawn()` 仅从 freeCells 随机生成食物，不含碰撞逻辑
5. Controller::update() 调 spawn 的前提是 `isFoodEaten()`，不再无条件调
6. 手工冒烟正常

---

## 提交格式

```
改动文件清单:
  重命名:
    src/controller/components/FoodComponent.h  → src/controller/FoodSpawner.h
    src/controller/components/FoodComponent.cpp → src/controller/FoodSpawner.cpp
  删除:
    src/controller/ICollisionDetector.h
    src/controller/NaiveCollisionDetector.h/.cpp
    src/controller/GridCollisionDetector.h/.cpp
    tests/test_collision_detector.cpp
  修改:
    src/controller/CMakeLists.txt
    tests/CMakeLists.txt
    src/controller/GameController.h/.cpp
    tests/test_components.cpp

新增/修改测试:
(删除 17 个 detector 用例，既有测试适配)

一句话冒烟:
编译 /W4 零警告，46 测试全绿，SnakeArena.exe 单人正常。
```
