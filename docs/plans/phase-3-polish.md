# Phase-3 打磨：UI 重构 + 碰撞系统重构 开发设计文档

> 版本：v0.1（待批准）
> 作者：主程
> 状态：草案，等待用户批准
> 关联：`docs/design.md`、`docs/architecture.md`、`docs/prototypes/ui-mockup-v4.html`

---

## 1. 目标

在不引入 AI / network / replay 模块的前提下，完成两个独立重构：

| 重构 | 目标 |
|---|---|
| **碰撞系统重构** | 将碰撞检测从"检测+响应耦合"拆分为纯检测器 + 独立解析器，支持策略模式（小棋盘/大棋盘自动切换），统一检测蛇-蛇、蛇-食物、蛇-障碍物 |
| **UI 重构** | 将游戏界面从"仅有棋盘"升级为"HUD 信息栏 + 棋盘 + 排名侧栏"，蛇身渐变色/发光、食物光晕、网格线、反光倒计时文字 |

---

## 2. 范围

### In Scope

**碰撞系统：**
- `ICollisionDetector` 抽象接口 + `NaiveCollisionDetector` + `GridCollisionDetector` 两套策略
- `CollisionReport` 数据结构和 `CollisionType` 枚举（6 种）
- `CollisionResolver` 两阶段处理（死亡优先 → 食物后处理）
- `MoveComponent` 吸收 freeCells 维护职责
- `FoodComponent` 拆分为 `FoodSpawner`（仅生成）+ CollisionDetector（检测吃食物）
- `Board` 增加 `obstacles` 预留字段
- `GameController::update()` 调整为 6 步流水线
- 旧 `CollisionComponent` 删除

**UI：**
- `SnakeItem`：蛇身渐变色（head → tail HSL 插值）+ 霓虹发光效果（QPainter blur）
- `FoodItem`：食物光晕（QPainter blur）
- `GameScene`：网格线渲染
- HUD `InfoBar`：Score / Length / Time / Speed / Kills / Rank
- 排名侧栏 `LeaderboardWidget`：实时排名 + 颜色标识 + 死亡划线
- `GameOverWidget`：多人排名表
- 倒计时文字反光动画：`QPropertyAnimation` + `QGraphicsOpacityEffect` 或 `QPalette` 渐变
- QSS 新增/调整：HUD、侧栏、多人结算、设置页 Tab 自定义
- 设置页：`QTabWidget` → 自定义按钮 + `QStackedWidget`

### Out of Scope

- AI / Network / Replay 模块
- 音效落地
- 多语言 i18n
- 实际障碍物地图
- QSS 半透明 `rgba()` 在 Qt 中的兼容（独立 hotfix）

---

## 3. 碰撞系统架构

### 3.1 核心数据结构

```cpp
// 碰撞类型
enum class CollisionType {
    None,
    Wall,         // head 出界
    SelfBody,     // head 撞自己身体
    OtherBody,    // head 撞另一条蛇的身体
    HeadToHead,   // 两条蛇的 head 在同一格
    Food,         // head 在食物格上
    Obstacle,     // head 在障碍物格上（预留）
};

// 一次碰撞事件
struct CollisionReport {
    int snakeIndex;              // 谁撞了
    CollisionType type;          // 撞了什么
    Point position;              // 碰撞位置
    int otherSnakeIndex = -1;    // 如果涉及另一条蛇，它是谁
};
```

### 3.2 策略接口

```cpp
class ICollisionDetector {
public:
    virtual ~ICollisionDetector() = default;
    virtual std::vector<CollisionReport> detect(const GameState& state) const = 0;
};
```

### 3.3 策略选择

```cpp
std::unique_ptr<ICollisionDetector> createDetector(const Board& board) {
    constexpr int kThreshold = 2500;  // 50×50 = 2500 cells
    if (board.width * board.height <= kThreshold)
        return std::make_unique<NaiveCollisionDetector>();
    else
        return std::make_unique<GridCollisionDetector>(board.width, board.height);
}
```

策略切换时机：`GameController::setBoardSize()` 时重建 detector。

### 3.4 NaiveCollisionDetector（方案 A）

O(N×S) 双层遍历，零额外内存。

```
for 每条蛇 i:
  head = snakes[i].head()
  // 1. Wall
  if (outOfBounds(head)) → report Wall, continue
  // 2. Food
  if (head == foodPos) → report Food
  // 3. Obstacle
  if (head in obstacles) → report Obstacle
  // 4. Body / HeadToHead
  for 每条蛇 j:
    for 每个身体段 k (跳过 j==i 且 k==0):
      if (head == body[j][k]):
        分类 → report, goto next snake
```

### 3.5 GridCollisionDetector（方案 C）

O(N+S)，O(W×H) 内存。使用 epoch 标记避免每 tick 全量清零。

```cpp
struct CellInfo {
    int headSnakeIdx = -1;
    int bodySnakeIdx = -1;
    int totalSegs = 0;
};

// build: 遍历所有段，epoch 判断首次访问时清零
// detect: O(1) per head，CellInfo 直接回答碰撞类型
```

### 3.6 CollisionResolver 两阶段

```
Pass 1: 处理所有死亡类型（Wall/SelfBody/OtherBody/HeadToHead/Obstacle）
  → markDead() 对应蛇
  → HeadToHead 两条都标记

Pass 2: 处理 Food
  → 仅对 isAlive() == true 的蛇生效
  → 一份食物只给第一条活蛇 → grow(), foodEaten=true, score+=foodPoints
```

### 3.7 MoveComponent 吸收 freeCells

当前 freeCells 维护分散在三处。重构后：

```cpp
void MoveComponent::moveAll(GameState& state) {
    for (auto& snake : state.snakes) {
        Point oldTail = snake.move();
        if (oldTail != snake.head())  // 没增长，旧尾释放
            state.board.freeCells.insert(oldTail);
        state.board.freeCells.erase(snake.head());  // 新头占领
    }
}
```

`freeCells` 仅由 `FoodSpawner` 消费（生成食物时随机选空格）。

### 3.8 Controller 新流水线

```
1. RenderComponent      → syncFromState(state)
2. InputComponent       → applyDirections(state)
3. MoveComponent        → moveAll(state) + 维护 freeCells
4. CollisionDetector    → detect(state) → vector<CollisionReport>
5. CollisionResolver    → resolve(state, reports)
6. FoodSpawner          → 如果 foodEaten，spawn(state, rng)
```

### 3.9 Board 变更

```cpp
struct Board {
    int width, height;
    std::unordered_set<Point> freeCells;    // 仅 FoodSpawner 使用
    std::unordered_set<Point> obstacles;    // 预留：地图障碍物
    Point foodPos;
    int foodPoints = 1;
    bool foodEaten = false;                 // 新增：本 tick 食物是否被吃
};
```

---

## 4. UI 变更规格

### 4.1 SnakeItem：渐变 + 发光

```
paint():
  // 计算当前段在蛇身中的位置 t = segmentIndex / totalLength
  // headColor=#00ff88, tailColor=#00bb55
  // 当前段颜色 = lerp(headColor, tailColor, t)
  // QPainter::setBrush(当前段颜色)
  // if (isHead): QPainter 设置 blur 效果 (QGraphicsDropShadowEffect 或手动 shadow)
```

Qt 中发光效果的实现路径：使用 `QGraphicsDropShadowEffect` 附加到 `SnakeItem`，或直接在 `paint()` 中用半透明 QPen 画多层同心矩形模拟 glow。优先使用 `QGraphicsDropShadowEffect`（更简洁）。

### 4.2 FoodItem：光晕

同上，`QGraphicsDropShadowEffect` + `#ff4466`。

### 4.3 GameScene：网格线

在 `syncFromState()` 中绘制。方案：创建 `QGraphicsLineItem` 网格线集合（一次性创建，resize 时重建），或直接在 `drawBackground()` 中画线。

推荐 `drawBackground()` override：不创建额外 item，纯绘制，性能最优。

### 4.4 HUD InfoBar

新组件 `InfoBar`（QWidget），嵌入 `GamePage`，位于 GameView 上方。

字段：
- Score（绿色高亮）
- Length（白色）
- Time（白色，mm:ss 格式）
- Speed（白色，SLOW/MID/FAST）
- Kills（红色，仅多人/AI 模式显示）
- Rank（绿色，仅多人/AI 模式显示，格式 "1 / 4"）

实现：水平 layout + divider 分隔线 + QLabel。通过 `setVisible()` 按模式控制 Kills/Rank 显隐。

### 4.5 LeaderboardWidget

新组件，位于棋盘右侧，宽度 ~170px。

内容：
- 标题 "— RANK —"
- 行列表：排名号、颜色圆点、玩家名、分数
- 自己高亮边框
- 死亡玩家划线 + 灰色
- 底部存活统计 "Alive: 3 / 4"

单人和 AI 模式：侧栏始终可见。单人模式仅显示一条自己。

### 4.6 GameOverWidget：多人排名表

结算面板中增加 `QTableWidget` 或在 QLabel 中用 HTML 表格渲染。

列：排名 / 玩家 / 分数 / 长度 / 击杀 / 状态（存活/死亡）。

单人或只有自己时，退化为当前两行（分数大字 + 统计小字）。

### 4.7 倒计时反光动画

实现路径：`QPropertyAnimation` 驱动 `QGraphicsOpacityEffect` 在文字上水平移动透明遮罩，或使用 `QPalette` + `QTimer` 逐帧刷渐变色。

推荐简洁方案：不使用 QSS 动画（Qt Widgets 不支持 CSS animation）。改为在 `showCountdown()` 中启动 `QTimer`（40ms interval），每帧更新 label 的 `QPalette` 中的 `QLinearGradient`，模拟亮带扫过效果。倒计时结束或切换状态时停止 timer。

```cpp
// 原理示意
QTimer shineTimer;
int shinePhase = 0;
connect(&shineTimer, &QTimer::timeout, [&]() {
    QLinearGradient g(shinePhase - 100, 0, shinePhase + 100, 0);
    g.setColorAt(0.0, #556);
    g.setColorAt(0.5, #00ff88);
    g.setColorAt(1.0, #556);
    QPalette pal = label->palette();
    pal.setBrush(QPalette::WindowText, g);  // 不直接支持，需 QPainter 自绘
    ...
});
```

> **注意**：QPalette 不支持 QLinearGradient。需改用自定义 QLabel 子类 `ShineLabel`，override `paintEvent()` 用 QPainter 绘制渐变文字。这是实现反光效果的唯一干净路径。

### 4.8 设置页 Tab 重构

`QTabWidget` → 自定义 QPushButton × 2 + `QStackedWidget`。

Tab 按钮使用 QSS 属性选择器 `[tabActive="true"]` 切换高亮下划线。

### 4.9 GamePage 布局变更

```
Before:
  GamePage
    └─ GameView (填满)

After:
  GamePage (QVBoxLayout)
    ├─ InfoBar (固定高度 ~50px)
    └─ QHBoxLayout
         ├─ GameView (弹性)
         └─ LeaderboardWidget (固定宽度 ~170px)
```

---

## 5. 文件变更清单

### 新增文件

```
src/core/CollisionReport.h         (enum CollisionType + struct CollisionReport)
src/controller/ICollisionDetector.h       (抽象接口)
src/controller/NaiveCollisionDetector.h   (方案 A)
src/controller/NaiveCollisionDetector.cpp
src/controller/GridCollisionDetector.h    (方案 C)
src/controller/GridCollisionDetector.cpp
src/controller/CollisionResolver.h
src/controller/CollisionResolver.cpp
src/controller/FoodSpawner.h
src/controller/FoodSpawner.cpp
src/app/InfoBar.h
src/app/InfoBar.cpp
src/app/LeaderboardWidget.h
src/app/LeaderboardWidget.cpp
src/app/ShineLabel.h
src/app/ShineLabel.cpp
tests/test_collision_detector.cpp
tests/test_collision_resolver.cpp
```

### 修改文件

```
src/core/Board.h                   (+obstacles, +foodEaten)
src/controller/GameController.h    (持有新组件，更新流水线)
src/controller/GameController.cpp
src/controller/MoveComponent.h     (吸收 freeCells 维护)
src/controller/MoveComponent.cpp
src/controller/FoodComponent.h → src/controller/FoodSpawner.h  (重命名+删检测)
src/controller/FoodComponent.cpp → src/controller/FoodSpawner.cpp
src/controller/RenderComponent.h   (无变化，适配新流水线)
src/controller/InputComponent.h    (无变化)
src/ui/SnakeItem.h                 (渐变色 + 发光)
src/ui/SnakeItem.cpp
src/ui/FoodItem.h                  (光晕)
src/ui/FoodItem.cpp
src/ui/GameScene.h                 (网格线)
src/ui/GameScene.cpp
src/ui/GameView.h                  (无变化)
src/app/GamePage.h                 (新布局)
src/app/GamePage.cpp
src/app/GameOverWidget.h           (多人排名表)
src/app/GameOverWidget.cpp
src/app/SettingsWidget.h           (Tab 重构)
src/app/SettingsWidget.cpp
src/resources/style/main.qss       (HUD、侧栏、结算、设置新增样式)
tests/CMakeLists.txt               (新增测试文件)
```

### 删除文件

```
src/controller/components/CollisionComponent.h
src/controller/components/CollisionComponent.cpp
```

---

## 6. 测试策略

| 测试文件 | 内容 |
|---|---|
| `test_collision_detector.cpp` | NaiveDetector: Wall/SelfBody/OtherBody/HeadToHead/Food/Obstacle/无碰撞；GridDetector 同用例；两者输出一致性对比 |
| `test_collision_resolver.cpp` | 两阶段：死亡优先、死蛇不吃食物、HeadToHead 双死、单蛇存活吃食物 |
| 既有测试 | 全部保持通过；适配 MoveComponent 新签名和 FoodComponent 重命名 |

---

## 7. 任务拆分

| ID | 标题 | 领域 | 预计改动幅度 | 依赖 |
|---|---|---|---|---|
| T-11 | 碰撞系统重构：数据结构 + 接口 | Collision | 小 | — |
| T-12 | NaiveCollisionDetector 实现 + 单元测试 | Collision | 中 | T-11 |
| T-13 | GridCollisionDetector 实现 + 单元测试 | Collision | 中 | T-11 |
| T-14 | CollisionResolver 实现 + 单元测试 | Collision | 中 | T-11 |
| T-15 | MoveComponent 吸收 freeCells + FoodSpawner 拆分 | Collision | 中 | T-11 |
| T-16 | Controller 流水线重构 + 旧 CollisionComponent 删除 | Collision | 中 | T-12~T-15 |
| T-17 | SnakeItem 渐变色 + 发光 | UI | 小 | — |
| T-18 | FoodItem 光晕 + GameScene 网格线 | UI | 小 | — |
| T-19 | InfoBar + LeaderboardWidget 实现 | UI | 中 | — |
| T-20 | GameOverWidget 多人排名表 | UI | 小 | — |
| T-21 | ShineLabel 反光文字 | UI | 小 | — |
| T-22 | SettingsWidget Tab 重构 + QSS 收尾 | UI | 中 | — |

执行顺序：**严格串行**（T-11 → T-22）。碰撞系统先行（架构基础），UI 后行（视觉层）。

---

## 8. 风险

| 风险 | 缓解 |
|---|---|
| Collision 重构导致既有测试大量失败 | T-11~T-16 每卡完成后即跑全量测试，及时修复 |
| `QGraphicsDropShadowEffect` 性能不足 | 降级为 paint() 中手动绘制半透明同心矩形 |
| Qt Widgets 不支持 CSS animation，反光效果实现复杂 | 已确定 `ShineLabel` + `QTimer` + QPainter 方案 |
| QSS `rgba()` 在 Qt5 中不生效 | 已在已知问题中；本阶段不修，后续独立 hotfix |
| 多人 UI 元素在单人模式下隐藏逻辑复杂 | InfoBar / LeaderboardWidget 提供 `setMode()` 接口统一控制 |
