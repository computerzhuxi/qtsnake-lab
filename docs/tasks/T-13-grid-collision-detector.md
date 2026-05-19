# T-13：GridCollisionDetector 实现 + 单元测试

> 签发：主程 | 日期：2026-05-19 | 状态：待领取

---

## 任务描述

实现 `GridCollisionDetector`，继承 `ICollisionDetector`，使用 2D 网格 + epoch 标记算法（O(N+S)），用于大棋盘（>50×50）。在既有 `test_collision_detector.cpp` 中新增 8 个 Grid 专用用例，再加 1 个跨策略一致性用例验证 Naive 与 Grid 对随机状态输出一致。

---

## 输入约束

### 允许修改/新增的文件

```
+ src/controller/GridCollisionDetector.h   (新增)
+ src/controller/GridCollisionDetector.cpp (新增)
~ src/controller/CMakeLists.txt            (追加 GridCollisionDetector.cpp)
~ tests/test_collision_detector.cpp        (追加 9 个 Grid 用例)
```

### 不允许做的事

- 不得修改 `ICollisionDetector.h`、`NaiveCollisionDetector.h/.cpp`
- 不得修改 `CollisionReport.h`、`Board.h`、`GameState.h`、`Snake.h`
- 不得修改既有 controller 组件或 GameController 流水线
- 不得修改 `tests/CMakeLists.txt`（test_collision_detector.cpp 已在 T-12 加入）
- 不得在 GridCollisionDetector 中引入 Qt 头文件
- 不得使用 `sleep` / `this_thread::sleep_for`

---

## 输出约束

### 1. `GridCollisionDetector.h`

```cpp
#include "ICollisionDetector.h"
#include <vector>

struct CellInfo {
    int headCount = 0;
    int firstHeadIdx = -1;    // 第一个蛇头的 snakeIndex
    int secondHeadIdx = -1;   // 第二个蛇头的 snakeIndex（用于 HeadToHead 报告）
    int bodySnakeIdx = -1;    // 占据此格的身体段所属蛇
    int totalSegs = 0;
};

class GridCollisionDetector : public ICollisionDetector {
public:
    explicit GridCollisionDetector(int boardWidth, int boardHeight);
    std::vector<CollisionReport> detect(const GameState& state) const override;

private:
    int m_width, m_height;
    mutable int m_epoch = 0;
    mutable std::vector<std::vector<CellInfo>> m_grid;
    mutable std::vector<std::vector<int>> m_epochGrid;
};
```

- 构造函数接收棋盘宽高，分配 `[height][width]` 的 grid + epochGrid
- `mutable` 允许 `detect() const` 内修改 epoch / grid（外部语义仍为纯函数）
- `CellInfo` 可放在头文件或 `.cpp` 内部

### 2. `GridCollisionDetector.cpp` — 检测算法

```
detect(state):
  m_epoch++
  // 如果棋盘尺寸变化，重新分配 grid
  ensureSize(board.width, board.height)

  // === Phase 1: Build occupancy grid ===
  for i in 0..snakes.size()-1:
    body = snakes[i].body()
    for k in 0..body.size()-1:
      p = body[k]
      if (m_epochGrid[p.y][p.x] != m_epoch):
        m_grid[p.y][p.x] = CellInfo{}          // 首次访问，清零
        m_epochGrid[p.y][p.x] = m_epoch
      cell = m_grid[p.y][p.x]
      cell.totalSegs++
      if (k == 0):                              // 这是蛇头
        if (cell.headCount == 0) cell.firstHeadIdx = i
        else if (cell.headCount == 1) cell.secondHeadIdx = i
        cell.headCount++
      else:                                     // 这是身体段
        cell.bodySnakeIdx = i

  // === Phase 2: Detect collisions ===
  reports = []
  for i in 0..snakes.size()-1:
    head = snakes[i].head()

    // 1. Wall
    if (head.x < 0 || head.x >= board.width ||
        head.y < 0 || head.y >= board.height):
      reports.push_back({i, Wall, head})
      continue

    cell = m_grid[head.y][head.x]

    // 2. HeadToHead
    if (cell.headCount >= 2):
      other = (cell.firstHeadIdx == i) ? cell.secondHeadIdx : cell.firstHeadIdx
      reports.push_back({i, HeadToHead, head, other})
      // continue? 不——食物和障碍物仍可检测（设计决策：HeadToHead 时跳过其他）
      // 本卡取 continue：头撞头意味着死亡，食物/障碍不再有意义
      continue

    // 3. SelfBody / OtherBody
    if (cell.totalSegs > cell.headCount):       // 除了头以外还有别的段
      if (cell.bodySnakeIdx == i):
        reports.push_back({i, SelfBody, head})
      else if (cell.bodySnakeIdx >= 0):
        reports.push_back({i, OtherBody, head, cell.bodySnakeIdx})
      // bodySnakeIdx == -1 理论上不会发生（totalSegs > headCount 但无 body）
      continue   // 身体碰撞后不检查食物/障碍

    // 4. Food
    if (head == board.foodPos):
      reports.push_back({i, Food, head})

    // 5. Obstacle
    if (board.obstacles.count(head)):
      reports.push_back({i, Obstacle, head})

  return reports
```

关键设计决策：
- Wall > HeadToHead > Body(Self/Other) > Food / Obstacle。前面命中后 `continue`，不报后面的。
- 这与 Naive 不同（Naive 中 Food 和 Body 可共存）。这是 Grid 策略的内部选择，不影响 Resolver 正确性——Resolver 已经两阶段处理，无论收到 1 份还是 2 份 Report 都能正确响应。
- `totalSegs > headCount` 判断是否有身体段在此格。

### 3. 新增测试用例（追加到 `test_collision_detector.cpp`）

9 个新用例：

| # | 用例名 | 场景 | 期望 |
|---|---|---|---|
| 1 | `gridNoCollision` | 同 noCollision，用 Grid(10,10) 检测 | reports 为空 |
| 2 | `gridWallCollision` | 同 wallCollision | 1 report: Wall |
| 3 | `gridSelfBodyCollision` | 同 selfBodyCollision | 1 report: SelfBody |
| 4 | `gridOtherBodyCollision` | 同 otherBodyCollision | 1 report: OtherBody |
| 5 | `gridHeadToHead` | 同 headToHead | 2 reports: HeadToHead |
| 6 | `gridFoodCollision` | 同 foodCollision | 1 report: Food |
| 7 | `gridObstacleCollision` | 同 obstacleCollision | 1 report: Obstacle |
| 8 | `gridFoodPlusBodyCollision` | 同 foodPlusBodyCollision | Grid 策略仅报 OtherBody（不报 Food） |
| 9 | `gridAndNaiveConsistent` | 100 个随机 GameState（蛇数 1~4，棋盘 10~50，随机蛇位置/方向/食物），分别用 Naive 和 Grid 检测 | 两个 detector 的 reports 在 CollisionType 级别一致（即每个 snakeIndex 的核心碰撞类型相同：死亡型 > Food）。详见下方说明。 |

> **用例 8 说明**：Grid 在 HeadToHead/Body 命中后 `continue`，不继续检查 Food。这与 Naive 的 "Food+Body 可共存" 不同。但 Resolver 两阶段（先 death 后 food）保证了最终游戏行为一致——身体碰撞已致死，Food 不会被处理。

> **用例 9 说明**：Grid 和 Naive 的 report **列表不一定逐条相同**（Grid 在某些场景少报 Food），但每个 `snakeIndex` 的 **核心碰撞结果** 一致。验证方式：
> - 对每个 snakeIndex，Grid 和 Naive 报告的 collision type 在 "Wall/HeadToHead/SelfBody/OtherBody" 级别一致
> - Food 的差异仅限于"同时报了 Body 时 Naive 多报一份 Food"，不影响游戏结果

用例 1-8 可直接复用 T-12 的 state 构造逻辑。用例 9 需要随机数生成（可用 `std::mt19937` 固定种子 42）。

---

## 验收标准

1. 编译通过（SnakeArena.exe + SnakeServer.exe），/W4 零警告
2. 54 个既有测试全部通过
3. 新增 9 个 Grid 测试全部通过（共计 63/63）
4. `GridCollisionDetector` 不依赖 Qt
5. epoch 清零逻辑正确：每 tick 只重置被蛇段占据的格子
6. 测试运行总时长 ≤ 30 秒
7. cross-validation（用例 9）100 个随机状态全部一致

---

## 提交格式（AGENTS.md §7）

```
改动文件清单:
+ src/controller/GridCollisionDetector.h
+ src/controller/GridCollisionDetector.cpp
~ src/controller/CMakeLists.txt            (追加 GridCollisionDetector.cpp)
~ tests/test_collision_detector.cpp        (追加 9 个 Grid 用例)

新增/修改测试:
tests/test_collision_detector.cpp::gridNoCollision           (new, pass)
tests/test_collision_detector.cpp::gridWallCollision         (new, pass)
tests/test_collision_detector.cpp::gridSelfBodyCollision     (new, pass)
tests/test_collision_detector.cpp::gridOtherBodyCollision    (new, pass)
tests/test_collision_detector.cpp::gridHeadToHead            (new, pass)
tests/test_collision_detector.cpp::gridFoodCollision         (new, pass)
tests/test_collision_detector.cpp::gridObstacleCollision     (new, pass)
tests/test_collision_detector.cpp::gridFoodPlusBodyCollision (new, pass)
tests/test_collision_detector.cpp::gridAndNaiveConsistent    (new, pass)

一句话冒烟:
编译 /W4 零警告，54 既有 + 9 新增 = 63/63 全绿，
gridAndNaiveConsistent 100 随机状态 cross-validation 全部通过。
```
