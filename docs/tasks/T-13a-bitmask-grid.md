# T-13a：增量位掩码网格（Board 管同步）+ growPending 清理迁移

> 签发：主程 | 日期：2026-05-19 | 状态：待领取
> 分支：`phase-3-collision-refactor`

---

## 任务描述

五件事：

1. **CellInfo** 嵌入 Board，替代 freeCells 做碰撞检测（freeCells 保留给食物生成）。Board 提供 `placeHead()` / `removeTail()` / `setFood()` 等同步方法。
2. **MoveComponent** 调用 Board 方法，不再直接操作 freeCells。
3. **CollisionComponent** 改为读 Board::grid 做位掩码检测，不再查 freeCells。
4. **Snake::move()** 不再清除 `m_growNext`，新增 `clearGrowPending()`。
5. **Controller** 末尾调用 `clearGrowPending()`。

---

## 输入约束

### 允许修改/新增的文件

```
~ src/core/Board.h          (新增 CellInfo + grid + placeHead/removeTail/setFood/clearFood)
~ src/core/Board.cpp        (新增，方法实现——如果需要 .cpp)
~ src/core/Snake.h          (新增 clearGrowPending() 声明)
~ src/core/Snake.cpp        (move() 不再清 m_growNext)
~ src/controller/components/MoveComponent.h   (移除 freeCells 操作)
~ src/controller/components/MoveComponent.cpp (调用 board.placeHead/removeTail)
~ src/controller/components/CollisionComponent.h/.cpp (改为读 board.grid)
~ src/controller/GameController.cpp (末尾调 clearGrowPending)
```

### 不允许做的事

- 不得修改 `FoodComponent`
- 不得修改 `ICollisionDetector`、`NaiveCollisionDetector`、`GridCollisionDetector`
- 不得引入第三方依赖
- 不得删除 `freeCells`（FoodComponent 仍用它）

---

## 输出约束

### 1. Board 变更

```cpp
// Board.h

struct CellInfo {
    uint8_t snakeMask = 0;   // bit i = 第 i 条蛇有身体段在此
    uint8_t headMask  = 0;   // bit i = 第 i 条蛇的头在此
    bool hasFood      = false;
    bool hasObstacle  = false;
};

struct Board {
    int width = 20;
    int height = 20;
    std::unordered_set<Point> freeCells;   // 保留，仅 FoodComponent 用
    std::vector<std::vector<CellInfo>> grid;  // ★ 新增：位掩码网格
    Point foodPos;
    int foodPoints = 1;
    std::unordered_set<Point> obstacles;   // 预留
    bool foodEaten = false;

    // -- 由 MoveComponent 调用 --
    void placeHead(int snakeIdx, Point pos) {
        grid[pos.y][pos.x].snakeMask |= (1 << snakeIdx);
        grid[pos.y][pos.x].headMask  |= (1 << snakeIdx);
        freeCells.erase(pos);
    }

    void removeTail(int snakeIdx, Point pos) {
        grid[pos.y][pos.x].snakeMask &= ~(1 << snakeIdx);
        // headMask 不碰——尾巴不可能是头
        if (grid[pos.y][pos.x].snakeMask == 0)
            freeCells.insert(pos);          // 仅当该格无其他蛇时释放
    }

    // -- 由 FoodSpawner（后续 T-15）调用 --
    void setFood(Point pos) {
        foodPos = pos;
        grid[pos.y][pos.x].hasFood = true;
    }

    void clearFood(Point pos) {
        grid[pos.y][pos.x].hasFood = false;
    }

    // -- 初始化 --
    void ensureGrid(int w, int h) {
        if (static_cast<int>(grid.size()) != h || (h > 0 && static_cast<int>(grid[0].size()) != w)) {
            width = w; height = h;
            grid.assign(h, std::vector<CellInfo>(w));
        }
    }
};
```

- 头文件内联实现，无需 `.cpp`
- `removeTail` 只在 `snakeMask == 0` 时才 insert freeCells——保证多蛇同格时不会误释放
- `ensureGrid` 在 MoveComponent 初始化/棋盘尺寸变更时调用

### 2. MoveComponent 变更

```cpp
// MoveComponent::update(state)
for (size_t i = 0; i < state.snakes.size(); ++i) {
    bool ate = state.snakes[i].growPending();    // ★ move() 前存
    Point oldTail = state.snakes[i].tail();       // ★ move() 前取旧尾
    state.snakes[i].move();                       // 不碰 m_growNext
    Point newHead = state.snakes[i].head();

    if (!ate)
        state.board.removeTail(static_cast<int>(i), oldTail);
    state.board.placeHead(static_cast<int>(i), newHead);
}

// 初始化
void MoveComponent::init(GameState& state) {
    state.board.ensureGrid(state.board.width, state.board.height);
    // 从蛇 body 全量构建 grid...
    for (size_t i = 0; i < state.snakes.size(); ++i)
        for (size_t k = 0; k < state.snakes[i].body().size(); ++k)
            if (k == 0) board.placeHead(i, ...)
            else        board.placeBody(i, ...)  // 或拆出一个 internal 方法
}
```

### 3. CollisionComponent 变更（过渡方案，T-15 删除）

```cpp
void CollisionComponent::update(GameState& state) {
    auto& board = state.board;

    for (size_t i = 0; i < state.snakes.size(); ++i) {
        Point head = state.snakes[i].head();

        // 1. Wall
        if (head.x < 0 || head.x >= board.width ||
            head.y < 0 || head.y >= board.height) {
            state.gameOver = true; return;
        }

        CellInfo& cell = board.grid[head.y][head.x];
        uint8_t others     = cell.snakeMask & ~(1 << i);
        uint8_t otherHeads = cell.headMask  & ~(1 << i);

        // 2. HeadToHead
        if (otherHeads) { state.gameOver = true; return; }
        // 3. SelfBody / OtherBody
        if (others)    { state.gameOver = true; return; }

        // 4. Food — 交给 FoodComponent 处理（当前不变）
        // 5. Obstacle
        if (cell.hasObstacle) { state.gameOver = true; return; }
    }
}
```

### 4. Snake 变更

```cpp
// Snake.h
void clearGrowPending() { m_growNext = false; }
```

```cpp
// Snake.cpp — move()
void Snake::move() {
    m_body.insert(m_body.begin(), nextHead());
    if (m_growNext) {
        // 不 pop_back
        // ★ 不再设 m_growNext = false
    } else {
        m_body.pop_back();
    }
}
```

### 5. Controller 变更

`update()` 末尾，FoodComponent 之后：

```cpp
for (auto& snake : m_gameState.snakes)
    snake.clearGrowPending();
```

---

## 验收标准

1. 编译 /W4 零警告
2. 63 个既有测试全部适配通过（CollisionComponent 测试需适配新 grid 逻辑，覆盖不减）
3. 增量 grid：增长时 tail 保留、不增长时 tail 释放；多蛇同格时 freeCells 不误释放
4. 位掩码碰撞判定与旧 freeCells 判定一致
5. `growPending` 在 Controller 末尾清理，不残留
6. 手工冒烟正常

---

## 提交格式

```
改动文件清单:
~ src/core/Board.h
~ src/core/Snake.h
~ src/core/Snake.cpp
~ src/controller/components/MoveComponent.h
~ src/controller/components/MoveComponent.cpp
~ src/controller/components/CollisionComponent.h
~ src/controller/components/CollisionComponent.cpp
~ src/controller/GameController.cpp

新增/修改测试:
(既有测试适配)

一句话冒烟:
编译 /W4 零警告，全量测试通过，SnakeArena.exe 单人模式正常。
```
