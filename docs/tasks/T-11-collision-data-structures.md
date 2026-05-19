# T-11：碰撞数据结构 + 接口定义

> 签发：主程 | 日期：2026-05-19 | 状态：待领取

---

## 任务描述

定义碰撞系统的三个基础设施：
1. `CollisionType` 枚举 + `CollisionReport` 数据结构
2. `ICollisionDetector` 抽象接口
3. `Board` 扩展（`obstacles` + `foodEaten`）

**不实现任何检测逻辑。**

---

## 输入约束

### 允许修改的文件

```
+ src/core/CollisionReport.h        (新增)
+ src/controller/ICollisionDetector.h (新增)
~ src/core/Board.h                  (修改：加 2 个字段)
```

### 不允许做的事

- 不得新建 `.cpp` 文件（头文件内联即可）
- 不得修改 `Board.h` 以外的任何既有文件
- 不得修改 `GameState.h`、`Snake.h`、`Point.h`、`Direction.h`
- 不得实现 `detect()` 方法
- 不得在 `CollisionReport.h` 中引入 Qt 头文件

---

## 输出约束

### 1. `src/core/CollisionReport.h`

```cpp
#ifndef SNAKE_CORE_COLLISIONREPORT_H
#define SNAKE_CORE_COLLISIONREPORT_H

#include "Point.h"

/// \brief 碰撞类型
enum class CollisionType {
    None,
    Wall,         // head 出界
    SelfBody,     // head 撞自己身体
    OtherBody,    // head 撞另一条蛇的身体
    HeadToHead,   // 两条蛇的 head 在同一格
    Food,         // head 在食物格上
    Obstacle,     // head 在障碍物格上（预留）
};

/// \brief 一次碰撞事件（纯数据）
struct CollisionReport {
    int snakeIndex;              // 哪条蛇发生了碰撞
    CollisionType type;          // 撞了什么
    Point position;              // 碰撞发生的格子坐标
    int otherSnakeIndex = -1;    // 如果涉及另一条蛇，它的索引（HeadToHead / OtherBody）
};

#endif
```

- 枚举值必须与上述 7 个一致
- `CollisionReport::otherSnakeIndex` 默认值 `-1`（表示不涉及其他蛇）

### 2. `src/controller/ICollisionDetector.h`

```cpp
#ifndef SNAKE_CONTROLLER_ICOLLISIONDETECTOR_H
#define SNAKE_CONTROLLER_ICOLLISIONDETECTOR_H

#include <vector>
#include "CollisionReport.h"

class GameState;

/// \brief 碰撞检测器抽象接口
/// \details 纯函数：输入 const GameState&，输出 vector<CollisionReport>。
///          不修改 GameState，不持有状态，不产生副作用。
class ICollisionDetector {
public:
    virtual ~ICollisionDetector() = default;
    virtual std::vector<CollisionReport> detect(const GameState& state) const = 0;
};

#endif
```

- 前置声明 `GameState`（不 `#include "GameState.h"`）
- `detect()` 为纯虚函数，`const` 修饰

### 3. `src/core/Board.h` 变更

在现有 `Board` struct 中追加两个字段：

```cpp
std::unordered_set<Point> obstacles;  // 预留：地图障碍物格
bool foodEaten = false;               // 本 tick 食物是否被吃
```

- `obstacles` 放在 `foodPos` 之后、`foodPoints` 之前（或 `foodPoints` 之后均可）
- `foodEaten` 放在 struct 末尾
- 不修改既有字段的类型、名字、顺序

---

## 验收标准

1. 项目编译通过（`SnakeArena.exe` + `SnakeServer.exe`），无新增警告
2. 46 个既有测试全部通过，无修改
3. `CollisionReport.h` 不依赖 Qt，`#include` 仅 `Point.h` + `<vector>`（在接口头文件中）
4. `ICollisionDetector.h` 不依赖 Qt，前置声明 `GameState`
5. `Board` 新字段不影响既有 `Board` 构造和序列化（均为默认值初始化）
6. 文件编码 UTF-8，换行 LF

---

## 提交格式（AGENTS.md §7）

```
改动文件清单:
+ src/core/CollisionReport.h
+ src/controller/ICollisionDetector.h
~ src/core/Board.h

新增/修改测试:
（无——本卡仅定义数据结构）

一句话冒烟:
编译通过，46 测试全绿，无新增警告。
```
