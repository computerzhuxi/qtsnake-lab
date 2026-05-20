# T-14：Board struct→class + Snake clearGrowPending

> 签发：主程 | 日期：2026-05-19 | 状态：待领取
> 分支：`phase-3-collision-refactor`

---

## 任务描述

1. **Board**：从 `struct` 升为 `class`——`freeCells` / `grid` 私有，提供公有方法。外部不再直接访问字段。
2. **Snake**：`move()` 不再清除 `m_growNext`，新增 `clearGrowPending()`。

---

## 输入约束

### 允许修改的文件

```
~ src/core/Board.h                    (struct→class，声明 CellInfo/方法)
+ src/core/Board.cpp                  (新增：构造/initFromSnakes/placeHead/placeBody/removeTail/setFood/clearFood/isOutOfBounds/cellAt)
~ src/core/Snake.h                    (新增 clearGrowPending 声明)
~ src/core/Snake.cpp                  (move() 不再清 m_growNext)
~ src/core/CMakeLists.txt             (追加 Board.cpp)
~ src/controller/components/MoveComponent.cpp   (适配 Board 新接口)
~ src/controller/components/CollisionComponent.cpp (适配)
~ src/controller/components/FoodComponent.cpp     (适配)
~ src/controller/GameController.cpp               (适配)
~ src/controller/RenderComponent.cpp              (适配，如有字段访问)
~ tests/test_components.cpp                       (适配)
~ tests/test_game_controller.cpp                  (适配)
~ tests/test_collision_detector.cpp               (适配)
```

### 不允许做的事

- 不得修改 `GameState.h`
- 不得修改 `ICollisionDetector`、`NaiveCollisionDetector`、`GridCollisionDetector`
- 不得修改 CMakeLists.txt（除 `src/core/CMakeLists.txt` 追加 Board.cpp）
- 不得引入新文件
- 不得删除任何文件

---

## 输出约束

### 1. Board.h

```cpp
#ifndef SNAKE_CORE_BOARD_H
#define SNAKE_CORE_BOARD_H

#include <unordered_set>
#include <vector>
#include <cstdint>
#include "Point.h"

class Board {
public:
    Board(int w = 20, int h = 20);

    // 尺寸
    int width() const  { return m_width; }
    int height() const { return m_height; }
    void setSize(int w, int h);        // resize + ensureGrid。保留旧名 resize 也可

    // 初始构建（从蛇身填充 grid + freeCells）
    void initFromSnakes(const std::vector<class Snake>& snakes);

    // MoveComponent 调用
    void placeHead(int snakeIdx, Point pos);
    void placeBody(int snakeIdx, Point pos);   // 构建时用，不含 headMask
    void removeTail(int snakeIdx, Point pos);

    // 查询（CollisionComponent / Controller 调用）
    bool isOutOfBounds(Point pos) const;
    struct CellInfo {
        uint8_t snakeMask = 0;
        uint8_t headMask  = 0;
        bool hasFood      = false;
        bool hasObstacle  = false;
    };
    const CellInfo& cellAt(Point pos) const;

    // 食物（FoodComponent / FoodSpawner 调用）
    Point foodPos() const              { return m_foodPos; }
    void setFood(Point pos);
    void clearFood(Point pos);
    int foodPoints() const             { return m_foodPoints; }
    bool isFoodEaten() const           { return m_foodEaten; }
    void markFoodEaten()               { m_foodEaten = true; }
    void clearFoodEaten()              { m_foodEaten = false; }

    // 空闲集（FoodSpawner 调用）
    const std::unordered_set<Point>& freeCells() const { return m_freeCells; }

    // 障碍物（预留）
    const std::unordered_set<Point>& obstacles() const { return m_obstacles; }
    void addObstacle(Point pos);

    // 分数
    int foodPoints() const             { return m_foodPoints; }
    void setFoodPoints(int pts)        { m_foodPoints = pts; }

private:
    struct CellInfo {
        uint8_t snakeMask = 0;
        uint8_t headMask  = 0;
        bool hasFood      = false;
        bool hasObstacle  = false;
    };

    int m_width = 20;
    int m_height = 20;
    std::unordered_set<Point> m_freeCells;
    std::vector<std::vector<CellInfo>> m_grid;
    Point m_foodPos;
    int m_foodPoints = 1;
    bool m_foodEaten = false;
    std::unordered_set<Point> m_obstacles;

    void ensureGrid(int w, int h);
};

#endif
```

**关键规则**：
- `removeTail` 只在 `snakeMask == 0` 时将该格 re-insert 到 `m_freeCells`（多蛇同格时不能提前释放）
- `placeHead` 总是 `snakeMask |= (1<<i)` + `headMask |= (1<<i)` + `freeCells.erase(pos)`
- `placeBody` 只设 `snakeMask`，不碰 `headMask`
- `setFood(pos)`：`m_foodPos = pos` + `m_grid[pos.y][pos.x].hasFood = true`
- `clearFood(pos)`：`m_grid[pos.y][pos.x].hasFood = false`
- `initFromSnakes`：遍历所有蛇所有段，调 `placeHead` / `placeBody` 构建初始状态

### 2. Snake 变更

```cpp
// Snake.h
void clearGrowPending() { m_growNext = false; }
Point tail() const;               // 返回 body().back()（如果有的话）
Point nextHead() const;           // 暴露给测试 / Board 构建
```

```cpp
// Snake.cpp
void Snake::move() {
    m_body.insert(m_body.begin(), nextHead());
    if (m_growNext) {
        // 不 pop_back
        // 不再设 m_growNext = false  ★
    } else {
        m_body.pop_back();
    }
}
```

### 3. 适配指引

所有 `board.width` → `board.width()`
所有 `board.height` → `board.height()`
所有 `board.freeCells` → `board.freeCells()`（只读）/ 通过 `placeHead/removeTail` 修改
所有 `board.foodPos` → `board.foodPos()`
所有 `board.foodEaten = true` → `board.markFoodEaten()`
所有直接修改 `freeCells` 的地方 → 调用 Board 方法

---

## 验收标准

1. 编译 /W4 零警告
2. 63 个既有测试全部适配通过，覆盖不减
3. Board 字段全部私有，外部只能通过方法访问
4. `initFromSnakes()` 构建的 grid 和 freeCells 一致：蛇占的格不在 freeCells，空格都在 freeCells
5. `placeHead` + `removeTail` 语义正确：增删后两个索引一致
6. `Snake::move()` 不碰 `m_growNext`
7. 手工冒烟正常

---

## 提交格式

```
改动文件清单:
~ src/core/Board.h
~ src/core/Snake.h
~ src/core/Snake.cpp
~ src/controller/components/MoveComponent.cpp
~ src/controller/components/CollisionComponent.cpp
~ src/controller/components/FoodComponent.cpp
~ src/controller/GameController.cpp
~ src/controller/RenderComponent.cpp  (如有)
~ tests/test_components.cpp
~ tests/test_game_controller.cpp
~ tests/test_collision_detector.cpp

新增/修改测试:
(既有测试适配，覆盖不减)

一句话冒烟:
编译 /W4 零警告，63 测试全绿，SnakeArena.exe 单人模式正常。
```
