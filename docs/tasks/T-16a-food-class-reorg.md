# T-16a：Food 升 class + Board 删食物 + 文件整理

> 签发：主程 | 日期：2026-05-19 | 状态：待领取
> 分支：`phase-3-collision-refactor`

---

## 任务描述

三件事一张卡：

1. **Food 升为 class**（复活 `core/Food.h`）：持有位置/分值/被吃状态，提供 `placeAt(board)` / `remove(board)` / `markEaten()` 等方法，接管 Board 中全部食物逻辑。
2. **Board 删食物字段**：删 `m_foodPos`/`m_foodPoints`/`m_foodEaten` 及对应方法；CellInfo 删 `hasFood`。Board 回归纯空间索引。
3. **文件整理**：RngService 从 core/ 移到 controller/；controller/components/ 打平到 controller/；删空目录 components/。

---

## 输入约束

### core/Food.h（新增——实际是复活旧文件，但内容全新）

```cpp
#ifndef SNAKE_CORE_FOOD_H
#define SNAKE_CORE_FOOD_H

#include "Point.h"

class Board;

class Food {
public:
    Food(int points = 1);

    Point position() const;
    int points() const;
    bool isEaten() const;

    void placeAt(Point pos, Board& board);
    void remove(Board& board);          // 从 grid 清除，不清 board 索引
    void markEaten();
    void clearEaten();

private:
    Point m_position;
    int m_points = 1;
    bool m_eaten = false;
};

#endif
```

```cpp
// core/Food.cpp
void Food::placeAt(Point pos, Board& board) {
    m_position = pos;
    board.setFoodFlag(pos, true);   // Board 只设一个 bool 标记
}

void Food::remove(Board& board) {
    board.setFoodFlag(m_position, false);
}
```

### Board 变更

- 删字段：`m_foodPos`、`m_foodPoints`、`m_foodEaten`
- 删方法：`foodPos()`、`setFood()`、`clearFood()`、`foodPoints()`、`setFoodPoints()`、`isFoodEaten()`、`markFoodEaten()`、`clearFoodEaten()`
- CellInfo 删 `hasFood`
- 新增：`void setFoodFlag(Point pos, bool value)`——仅设 `m_grid[pos].hasFood`，不维护其他状态

### GameState 变更

```cpp
struct GameState {
    Board board;
    std::vector<Snake> snakes;
    Food food;          // ★ 新增
    int score = 0;
    bool gameOver = false;
};
```

### 文件整理

```
移动: src/core/RngService.h → src/controller/RngService.h
移动: src/controller/components/CollisionComponent.h/.cpp → src/controller/
移动: src/controller/components/InputComponent.h/.cpp     → src/controller/
移动: src/controller/components/MoveComponent.h/.cpp     → src/controller/
移动: src/controller/components/RenderComponent.h/.cpp   → src/controller/
删除: src/controller/components/（空目录）
```

### 适配清单

所有引用旧路径/旧接口的地方：

| 文件 | 适配内容 |
|---|---|
| `Board.h` | 删食物相关声明；CellInfo 删 hasFood；加 setFoodFlag |
| `Board.cpp` | 删 setFood/clearFood 实现；setFoodFlag 实现 |
| `FoodSpawner.cpp` | `state.board.foodPos()` → `state.food.position()`；调 `food.placeAt(board)` |
| `CollisionComponent.cpp` | `cell.hasFood` → `head == state.food.position()` |
| `GameController.cpp` | `board.isFoodEaten()` → `state.food.isEaten()`；`board.markFoodEaten()` → `state.food.markEaten()`；删 initComponents 中 `m_food->init()` 的食物分值设置 |
| `GameController.h` | `#include "RngService.h"` 路径 |
| `GameScene.cpp` | `board.foodPos()` → `state.food.position()` |
| `test_components.cpp` | 适配 FoodSpawner 测试 |
| `test_board.cpp` | 适配 Board 测试（删食物相关断言） |
| 所有文件 | `#include "components/..."` → `#include "..."`；RngService 路径 |
| `CMakeLists.txt`（core + controller） | RngService 移除/追加；组件路径更新 |

---

## 验收标准

1. 编译 /W4 零警告
2. 46 测试全绿
3. Board 无任何食物字段/方法
4. Food class 持有全部食物状态
5. RngService 在 controller/
6. controller/ 无 components/ 子目录
7. 手工冒烟正常

---

## 提交格式

```
改动文件清单:
  core/Food.h/.cpp (重写)
  core/Board.h/.cpp (删食物)
  core/GameState.h (+food)
  core/CMakeLists.txt (-RngService)
  移动: core/RngService.h → controller/RngService.h
  移动: controller/components/* → controller/
  删除: controller/components/
  controller/CMakeLists.txt
  (其他适配文件)

一句话冒烟:
编译 /W4 零警告，46 测试全绿，SnakeArena.exe 单人正常。
```
