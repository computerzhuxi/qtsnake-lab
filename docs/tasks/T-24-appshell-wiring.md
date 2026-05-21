# T-24：AppShell/GamePage 接线（InfoBar + Leaderboard + GameOver 数据落地）

> 签发：主程 | 日期：2026-05-21 | 状态：待领取
> 分支：`phase-3-collision-refactor`
> 前置卡：T-23（statsUpdated 信号已实现）

---

## 任务描述

将 T-23 提供的 `statsUpdated` 信号接入 GamePage → InfoBar / LeaderboardWidget，并修复 GameOver 数据（kills/time 不再传 0）。使 InfoBar 六列数据实时反映游戏状态。

---

## 输入约束

### 允许修改的文件

```
~ src/app/AppShell.h       (新增 statsUpdated slot 声明)
~ src/app/AppShell.cpp     (connect statsUpdated → slot → GamePage)
~ src/app/GamePage.h       (新增 updateStats 公开方法)
~ src/app/GamePage.cpp     (updateStats 实现：分发到 InfoBar + LeaderboardWidget)
```

### 不允许做的事

- 不得修改 core / controller / ui 层
- 不得在 `src/app/` 中使用 `setStyleSheet`
- 不得修改 GamePage 布局骨架
- 不得使用 `using namespace std;`

---

## 输出约束

### 1. GamePage 新增公开方法

```cpp
/// \brief 每 tick 更新 UI 数据（InfoBar + Leaderboard）
void updateStats(int score, int length, int timeSec,
                 int speedMs, int kills, int rank, int totalPlayers);
```

实现：

```cpp
void GamePage::updateStats(int score, int length, int timeSec,
                           int speedMs, int kills, int rank, int totalPlayers) {
    if (m_infoBar) {
        m_infoBar->setScore(score);
        m_infoBar->setLength(length);
        m_infoBar->setTime(timeSec);
        m_infoBar->setSpeed(speedMs);
        m_infoBar->setKills(kills);
        m_infoBar->setRank(rank, totalPlayers);
    }

    if (m_leaderboard) {
        // 单人模式：仅显示自己
        PlayerInfo self;
        self.name = "YOU";
        self.score = score;
        self.alive = true;
        self.isSelf = true;
        m_leaderboard->updatePlayers({self});
    }
}
```

### 2. AppShell 接线

**AppShell.h** 新增 private slot：

```cpp
private slots:
    void onStatsUpdated(int score, int length, int timeSec,
                        int speedMs, int kills, int rank, int totalPlayers);
```

**AppShell.cpp `startSinglePlayer`** 追加 connect：

```cpp
connect(m_controller.get(), &GameController::statsUpdated,
        this, &AppShell::onStatsUpdated);
```

**onStatsUpdated 实现**：

```cpp
void AppShell::onStatsUpdated(int score, int length, int timeSec,
                              int speedMs, int kills, int rank, int totalPlayers) {
    m_currentScore = score;
    m_gamePage->updateStats(score, length, timeSec, speedMs, kills, rank, totalPlayers);
}
```

### 3. 修复 GameOver 数据

当前 `onControllerStateChanged(GameOver)` 传 `kills=0, time=0`：

```cpp
// 旧代码：
m_gamePage->showGameOver(m_currentScore, len, 0, 0);

// 改为从 GameController 取真实数据：
const auto& gs = m_controller->gameState();
int len = gs.snakes.empty() ? 0 : static_cast<int>(gs.snakes[0].body().size());
int kills = ???  // 需要从 Controller 取
int timeSec = ??? // 需要从 Controller 取
```

**方案**：在 GameController 添加三个 getter：

T-23 已隐含这些数据在 Controller 中（`m_kills`, `m_elapsedSec`），T-23 需要额外暴露：

```cpp
// GameController.h 追加：
int kills() const { return m_kills; }
int elapsedSec() const { return m_elapsedSec; }
```

> 如果 T-23 已暴露这些 getter，则直接使用。否则 T-24 开发需补上（3 行代码，在 GameController.h 中追加两个 `int kills() const` / `int elapsedSec() const` 即可，视为 T-23 遗漏，本卡顺手补上）。

AppShell `onControllerStateChanged(GameOver)` 最终：

```cpp
case GameController::State::GameOver: {
    const auto& gs = m_controller->gameState();
    int len = gs.snakes.empty() ? 0 : static_cast<int>(gs.snakes[0].body().size());
    m_gamePage->showGameOver(m_currentScore, len,
                             m_controller->kills(),
                             m_controller->elapsedSec());
    break;
}
```

### 4. InfoBar 初始状态

`startSinglePlayer` 时重置 InfoBar 为初始值：

```cpp
m_gamePage->updateStats(0, 3, 0, m_controller->speedMs(), 0, 1, 1);
```

（snake 初始长度 = 3，由 startGame 中的 initializeSnake 决定）

### 5. 单人模式 LeaderboardWidget

LeaderboardWidget 在单人模式下显示一行：`#1 · YOU · score`，自己高亮、存活。

`updateStats` 已包含此逻辑。InfoBar 的 `setMode(false)` 已在 InfoBar 构造末尾调用——单人模式隐藏 Kills/Rank 列。**注意**：当前 InfoBar 默认 `setMode(false)` 隐藏了 Kills/Rank。如果产品需要单人模式也显示 Kills/Rank，把 `setMode` 参数改为 `true`。本卡不做此决策，保持现有行为。

---

## 验收标准

1. 编译 /W4 零警告
2. 45 测试全绿
3. 单人模式 Playing 中：InfoBar Score/Length/Time/Speed 实时更新（吃食物后 Score +10、Length +1）
4. InfoBar Time 从 00:00 开始递增
5. Speed 显示 "MID"（100ms tick）
6. 撞墙死亡后 GameOver 显示正确的 Score/Length/Kills/Time
7. LeaderboardWidget 显示一行 "YOU" 数据
8. `src/app/` 中零 `setStyleSheet`

---

## 提交格式

```
改动文件清单:
~ src/app/AppShell.h
~ src/app/AppShell.cpp
~ src/app/GamePage.h
~ src/app/GamePage.cpp
（如 GameController.h 需补 kills()/elapsedSec() getter，追加）

新增/修改测试列表:
无测试变更，45 全绿。

一句冒烟:
编译 /W4 零警告，45 测试全绿，SnakeArena.exe：单人模式 Playing 中
InfoBar Score/Length/Time/Speed 实时更新，吃食物后 Score+10 Length+1，
撞墙 GameOver 显示正确数据，Leaderboard 显示 YOU 行。
```
