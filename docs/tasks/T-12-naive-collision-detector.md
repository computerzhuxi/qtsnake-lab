# T-12：NaiveCollisionDetector 实现 + 单元测试

> 签发：主程 | 日期：2026-05-19 | 状态：待领取

---

## 任务描述

实现 `NaiveCollisionDetector`，继承 `ICollisionDetector` 接口，使用双层遍历算法（O(N×S)）检测所有碰撞类型。新建 `test_collision_detector.cpp`，覆盖 6 种碰撞 + 无碰撞 + 多报告场景。

---

## 输入约束

### 允许修改/新增的文件

```
+ src/controller/NaiveCollisionDetector.h   (新增)
+ src/controller/NaiveCollisionDetector.cpp (新增)
+ tests/test_collision_detector.cpp         (新增)
~ tests/CMakeLists.txt                      (追加 test_collision_detector.cpp)
~ src/controller/CMakeLists.txt             (追加 NaiveCollisionDetector.cpp)  ← 主程补授权
~ src/controller/ICollisionDetector.h       (class GameState → struct GameState)  ← 主程补授权
```

### 不允许做的事

- 不得修改 `CollisionReport.h`、`Board.h`、`GameState.h`、`Snake.h`
- 不得修改既有 controller 组件（MoveComponent、FoodComponent 等）
- 不得修改 `GameController` 或其流水线
- 不得引入 Qt 依赖到 NaiveCollisionDetector（仅依赖 core + ICollisionDetector）
- 不得新建除上述 4 个外的任何文件
- 不得使用 `sleep` / `this_thread::sleep_for` 推进测试

---

## 输出约束

### 1. `NaiveCollisionDetector.h`

```cpp
#include "ICollisionDetector.h"

class NaiveCollisionDetector : public ICollisionDetector {
public:
    std::vector<CollisionReport> detect(const GameState& state) const override;
};
```

### 2. `NaiveCollisionDetector.cpp` — 检测算法

```
detect(state):
  reports = []

  for i in 0..snakes.size()-1:
    head = snakes[i].head()

    // 1. Wall
    if (head.x < 0 || head.x >= board.width ||
        head.y < 0 || head.y >= board.height):
      reports.push_back({i, Wall, head})
      continue  // 出界后不检查其他碰撞

    // 2. Food（不互斥，可与身体碰撞并存）
    if (head == board.foodPos):
      reports.push_back({i, Food, head})

    // 3. Obstacle（预留）
    if (board.obstacles.count(head)):
      reports.push_back({i, Obstacle, head})

    // 4. Body / HeadToHead
    for j in 0..snakes.size()-1:
      body = snakes[j].body()
      start = (i == j) ? 1 : 0   // 跳过自己的头
      for k = start .. body.size()-1:
        if (head == body[k]):
          if (i != j && k == 0):
            reports.push_back({i, HeadToHead, head, j})
          else if (i == j):
            reports.push_back({i, SelfBody, head})
          else:
            reports.push_back({i, OtherBody, head, j})
          goto next_snake   // 每个头只报告一次身体碰撞

    next_snake: ;

  return reports
```

- 每个蛇头最多产生：1 个 Wall（提前 continue）+ 1 个 Food + 1 个 Obstacle + 1 个 Body/HeadToHead。单头最多 4 份 report。
- `continue` 仅在 Wall 分支使用——出界后不再检查食物/障碍/身体。
- Food 和 Body 碰撞可以同时存在（蛇头吃食物的同时撞到另一条蛇身体）。

### 3. `test_collision_detector.cpp` — 测试用例

最少 8 个用例，命名空间/类名自由：

| # | 用例名 | 场景 | 期望 |
|---|---|---|---|
| 1 | `noCollision` | 单蛇，head 在安全位置，freeCells 非空 | reports 为空 |
| 2 | `wallCollision` | 单蛇，head 在 (0,0) 且 `Direction::Left`（模拟 move 后头出界） | 1 report: Wall |
| 3 | `selfBodyCollision` | 单蛇 body = {(3,3),(4,3),(3,3),(5,3)}，head 在 (3,3) 与 body[2] 重合 | 1 report: SelfBody |
| 4 | `otherBodyCollision` | 双蛇：蛇 0 head=(5,5)，蛇 1 body={(6,5),(5,5),(4,5)}，蛇 0 head==蛇 1 body[1] | 1 report: OtherBody, otherSnakeIndex=1 |
| 5 | `headToHead` | 双蛇：蛇 0 head=(7,7)，蛇 1 head=(7,7) | 2 reports: 各一条 HeadToHead |
| 6 | `foodCollision` | 单蛇 head == foodPos | 1 report: Food |
| 7 | `obstacleCollision` | 单蛇 head 在 obstacles 集合中 | 1 report: Obstacle |
| 8 | `foodPlusBodyCollision` | 单蛇 head 同时==foodPos 且撞另一蛇身体 | 2 reports: Food + OtherBody |

> **构造 GameState 的技巧**：测试直接构造 `GameState`、填充 `snakes`（含 body）、设置 `board.width/height/foodPos/obstacles`。不需要 Controller、不需要 `move()`、不需要 Qt 事件循环。碰撞检测是纯函数——蛇 body 中已包含 move 后的新头位置。

---

## 验收标准

1. 编译通过（SnakeArena.exe + SnakeServer.exe），/W4 零警告
2. 46 个既有测试全部通过，不受影响
3. 新增 8 个碰撞检测测试全部通过
4. `NaiveCollisionDetector` 不依赖 Qt（`#include` 仅 core + ICollisionDetector）
5. 测试运行总时长 ≤ 30 秒
6. 算法与 §2 伪代码一致：Wall 用 `continue` 短路，身体碰撞用 `goto` 每头只报告一次

---

## 提交格式（AGENTS.md §7）

```
改动文件清单:
+ src/controller/NaiveCollisionDetector.h
+ src/controller/NaiveCollisionDetector.cpp
+ tests/test_collision_detector.cpp
~ tests/CMakeLists.txt

新增/修改测试:
tests/test_collision_detector.cpp (new, 8 cases, all pass)

一句话冒烟:
编译 /W4 零警告，46 既有 + 8 新增 = 54/54 全绿，
SnakeArena.exe 单人模式正常。
```
