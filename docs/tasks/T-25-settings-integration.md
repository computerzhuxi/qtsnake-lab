# T-25：Settings 集成（速度/大小/键位实际生效）

> 签发：主程 | 日期：2026-05-21 | 状态：待领取
> 分支：`phase-3-collision-refactor`
> 前置卡：T-24（InfoBar + Leaderboard + GameOver 数据已接线）

---

## 任务描述

让 SettingsWidget 的三个设置项实际作用于游戏：速度（tick 间隔）、棋盘大小、移动键位。当前 `startGame(20, 20, 100)` 参数硬编码，键位硬编码在 AppShell::keyPressEvent 中。

---

## 输入约束

### 允许修改的文件

```
~ src/app/SettingsWidget.h    (新增 getter 方法 + speed/size 枚举值常量)
~ src/app/SettingsWidget.cpp  (ComboBox 存当前选中值)
~ src/app/AppShell.h          (新增 settings 成员或直接读取)
~ src/app/AppShell.cpp        (startSinglePlayer 读取 Settings + keyPressEvent 支持 WASD)
```

### 不允许做的事

- 不得修改 core / controller / ui 层
- 不得在 `src/app/` 中使用 `setStyleSheet`
- 不得修改 GamePage
- 不得修改 SettingsWidget 的布局/样式（T-22 已完成）
- 不得使用 `using namespace std;`

---

## 输出约束

### 1. SettingsWidget 公开 getter

```cpp
// SettingsWidget.h 追加：
int speedMs() const;       // 返回当前选中的 tick 间隔（慢=150, 中=100, 快=60）
int boardSize() const;     // 返回当前选中的棋盘尺寸（小=15, 中=20, 大=30）
QString keyBinding() const; // 返回 "arrows" 或 "wasd"
```

**实现**：在 SettingsWidget 构造中给每个 QComboBox 设 objectName，getter 通过 `findChild<QComboBox*>` 读取 `currentIndex()` 映射为具体值。

映射表：

| ComboBox | Index 0 | Index 1 | Index 2 |
|---|---|---|---|
| 速度 | 150 (慢) | 100 (中) | 60 (快) |
| 棋盘大小 | 15 (小) | 20 (中) | 30 (大) |
| 键位 | arrows (方向键) | wasd | — |

**简化实现**：在 SettingsWidget 中新增三个 private 成员指针：
```cpp
QComboBox* m_speedCombo;
QComboBox* m_sizeCombo;
QComboBox* m_keyCombo;
```

构造中保存指针，getter 直接读 `currentIndex()` 映射。不依赖 findChild。

### 2. AppShell 读取 Settings 启动游戏

**startSinglePlayer** 修改：

```cpp
void AppShell::startSinglePlayer() {
    showGame();

    int speedMs  = m_settingsWidget->speedMs();
    int boardSize = m_settingsWidget->boardSize();

    m_controller = std::make_unique<GameController>(m_gamePage->scene());
    // ... connect 信号 ...

    m_controller->startGame(boardSize, boardSize, speedMs);
    m_gamePage->updateStats(0, 3, 0, speedMs, 0, 1, 1);
    // ...
}
```

### 3. WASD 键位支持

**AppShell::keyPressEvent** Playing 分支扩展：

```cpp
QString keyBinding = m_settingsWidget->keyBinding();
bool useWASD = (keyBinding == "wasd");

if (state == GameController::State::Playing) {
    int k = event->key();
    if ((!useWASD && k == Qt::Key_Up)    || (useWASD && k == Qt::Key_W))
        m_controller->input()->setDirection(Direction::Up);
    else if ((!useWASD && k == Qt::Key_Down)  || (useWASD && k == Qt::Key_S))
        m_controller->input()->setDirection(Direction::Down);
    else if ((!useWASD && k == Qt::Key_Left)  || (useWASD && k == Qt::Key_A))
        m_controller->input()->setDirection(Direction::Left);
    else if ((!useWASD && k == Qt::Key_Right) || (useWASD && k == Qt::Key_D))
        m_controller->input()->setDirection(Direction::Right);
    else if (k == Qt::Key_Escape) {
        m_controller->pause();
        m_gamePage->showPause();
    }
}
```

**注意**：Ready 状态下按任意键触发 `handleReadyKey()` 的逻辑不变——两个键位方案都只需要任意键。

### 4. main.qss

本卡不涉及 QSS 变更。T-22 已完成 Settings 全部样式。

---

## 验收标准

1. 编译 /W4 零警告
2. 45 测试全绿
3. Settings 速度选择"慢(150ms)"→ 游戏蛇明显变慢；"快(60ms)"→ 明显变快
4. Settings 棋盘选择"小(15×15)"→ 游戏区域格子减少；"大(30×30)"→ 格子增多
5. Settings 键位选 WASD → W/A/S/D 控制方向；选方向键 → ↑↓←→ 控制
6. 设置页"返回"后设置值保留，下次进设置页仍选中上次选项
7. `src/app/` 中零 `setStyleSheet`

---

## 提交格式

```
改动文件清单:
~ src/app/SettingsWidget.h
~ src/app/SettingsWidget.cpp
~ src/app/AppShell.h
~ src/app/AppShell.cpp

新增/修改测试列表:
无测试变更，45 全绿。

一句冒烟:
编译 /W4 零警告，45 测试全绿，SnakeArena.exe：
设置 → 速度/大小/键位切换 → 游戏中实际生效，
WASD 控制方向流畅，棋盘大小正确，src/app/ 零 setStyleSheet。
```
