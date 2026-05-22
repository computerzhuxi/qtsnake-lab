# T-23：Controller 数据管道（kills + 时间 + stats 信号）

> 签发：主程 | 日期：2026-05-21 | 状态：待领取
> 分支：`phase-3-collision-refactor`
> 前置卡：T-22（Phase-3 UI 线全部完成）

---

## 任务描述

在 GameController 中补齐 UI 层所需的游戏数据：击杀计数、游戏时长、每 tick 推送 stats 信号。当前 Controller 已有 `scoreChanged` 信号，但 kills/time/snakeLength 均不跟踪，InfoBar 和 GameOver 拿到的数据全是 0。

---

## 输入约束

### 允许修改的文件

```
~ src/controller/GameController.h    (新增成员 + 信号)
~ src/controller/GameController.cpp  (processCollisions 计数 + update 计时 + emit 信号)
```

### 不允许做的事

- 不得修改 core / ui / app / logging / audio 层
- 不得修改 GameState（数据字段由 Controller 自身追踪）
- 不得修改 Snake / Board / Food
- 不得使用 `using namespace std;`

---

## 输出约束

### 1. 新增 GameController 成员

```cpp
private:
    int m_kills = 0;              ///< 本局击杀数
    qint64 m_gameStartMs = 0;     ///< 游戏开始时间戳（Playing 后首 tick 记录）
    int m_elapsedSec = 0;         ///< 已用秒数
```

### 2. 新增信号

```cpp
signals:
    /// \brief 每 tick 推送 UI 所需数据（InfoBar 等消费）
    /// \param score 当前分数
    /// \param length 玩家蛇长度（取 snakes[0]，若无则为 0）
    /// \param timeSec 已流逝秒数
    /// \param speedMs 当前速度（tick 间隔毫秒）
    /// \param kills 击杀数
    /// \param rank 排名（单人=1）
    /// \param totalPlayers 总玩家数（单人=1）
    void statsUpdated(int score, int length, int timeSec,
                      int speedMs, int kills, int rank, int totalPlayers);
```

名称 `statsUpdated`，每 tick 在 update() 末尾 emit。

### 3. Kills 计数逻辑

在 `processCollisions()` 中，当某个蛇因碰撞被设为 `alive = false` 时：

```cpp
case Wall:
case SelfBody:
case OtherBody:
case Obstacle:
    m_state.snakes[r.snakeIndex].alive = false;
    if (r.snakeIndex == 0) m_kills += 0;  // 自己被撞不计数
    // 谁是击杀者？对于 OtherBody——击杀者是 r.otherSnakeIndex 那条蛇
    // 简化：仅多人模式下 otherSnakeIndex >= 0 时给击杀者 +1
    break;
case HeadToHead:
    m_state.snakes[r.snakeIndex].alive = false;
    m_state.snakes[r.otherSnakeIndex].alive = false;
    // 双方皆死，各不计数（HeadToHead 对撞双方都死）
    break;
```

**简化规则**（当前仅单人模式，但为多人预留）：
- `OtherBody` 类型：击杀者 = `r.otherSnakeIndex`（那个身体的主人杀死了撞上来的蛇头）。如果击杀者是玩家蛇（snakeIndex==0），`m_kills++`。
- `Wall` / `SelfBody` / `Obstacle`：自杀，不计数。
- `HeadToHead`：对撞双亡，均不计数。

实现方式——在 `processCollisions` 最后追加一次遍历：

```cpp
// 统计击杀：OtherBody 中 otherSnakeIndex 是身体拥有者（击杀方）
for (auto& r : reports) {
    if (r.type == CollisionType::OtherBody && r.otherSnakeIndex == 0)
        m_kills++;
}
```

### 4. 时间跟踪逻辑

在 `update()` 中：

```cpp
// 每 tick 更新耗时（仅在 Playing 状态）
if (m_phase == State::Playing) {
    if (m_gameStartMs == 0) {
        m_gameStartMs = QDateTime::currentMSecsSinceEpoch();
    }
    m_elapsedSec = static_cast<int>(
        (QDateTime::currentMSecsSinceEpoch() - m_gameStartMs) / 1000);
}
```

`startGame()` 中重置：`m_gameStartMs = 0; m_elapsedSec = 0; m_kills = 0;`

### 5. emit statsUpdated

在 `update()` 末尾（processCollisions + FoodSpawner 之后）：

```cpp
// Emit stats for UI
int len = m_state.snakes.empty() ? 0
          : static_cast<int>(m_state.snakes[0].body().size());
emit statsUpdated(m_state.score, len, m_elapsedSec,
                  m_speedMs, m_kills, 1, 1);
```

单人模式 rank=1, totalPlayers=1。多人模式暂时预留，后续改。

### 6. startGame 重置

```cpp
m_kills = 0;
m_gameStartMs = 0;
m_elapsedSec = 0;
```

---

## 验收标准

1. 编译 /W4 零警告
2. 45 测试全绿
3. `statsUpdated` 信号每 tick 正确发出（参数：score=当前分数, length=蛇身长度, timeSec=流逝秒数, speedMs=tick间隔, kills=击杀数, rank=1, total=1）
4. 吃一个食物后 score 正确递增，snake length 正确 +1
5. Kills 在单人模式下始终为 0（无其他蛇可杀）
6. `startGame()` 调用后 kills/time 重置为 0

---

## 提交格式

```
改动文件清单:
~ src/controller/GameController.h
~ src/controller/GameController.cpp

新增/修改测试列表:
无测试变更，45 全绿。

一句冒烟:
编译 /W4 零警告，45 测试全绿，statsUpdated 信号每 tick 正确发射。
```
