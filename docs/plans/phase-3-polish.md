# Phase-3 打磨：碰撞系统重构 + UI 重构 开发设计文档

> 版本：v1.0（最终锁定）
> 作者：主程
> 状态：已批准，实施中
> 分支：`phase-3-collision-refactor`
> 关联：`docs/design.md`、`docs/architecture.md`、`docs/prototypes/ui-mockup-v4.html`

---

## 1. 目标

| 重构 | 目标 |
|---|---|
| **碰撞系统重构** | Board 升为 class，内嵌增量位掩码网格（CellInfo）；CollisionComponent 退化为纯检测（出 reports，不管响应）；Controller 本体处理碰撞响应；freeCells 保留给食物生成 |
| **UI 重构** | HUD 信息栏 + 排名侧栏 + 蛇身渐变色/发光 + 食物光晕 + 网格线 + 反光倒计时 |

---

## 2. 范围

### In Scope（碰撞）

- `Board`：struct → class，内嵌 `CellInfo` + `grid`，私有 `freeCells`，公开 `placeHead/removeTail/setFood/clearFood/initFromSnakes`
- `Snake`：`move()` 不再清除 `m_growNext`，新增 `clearGrowPending()` 供 MoveComponent 读后清理
- `MoveComponent`：调 Board 方法维护 grid，不直接操作 freeCells/grid
- `CollisionComponent`：纯检测 `detect(state) → vector<CollisionReport>`，读 grid 做位掩码判断，不修改任何状态
- `Controller`：`update()` 内调 detect → 处理 reports（死亡/食物/gameOver 逻辑）
- `FoodComponent` → `FoodSpawner`：仅负责从 freeCells 生成食物
- 删除：`ICollisionDetector`、`NaiveCollisionDetector`、`GridCollisionDetector`

### In Scope（UI）

- `SnakeItem`：渐变色 + 霓虹发光
- `FoodItem`：光晕
- `GameScene`：网格线
- `InfoBar`：Score / Length / Time / Speed / Kills / Rank
- `LeaderboardWidget`：实时排名侧栏
- `GameOverWidget`：多人排名表
- `ShineLabel`：倒计时反光文字
- `SettingsWidget`：Tab 重构，QTabWidget → 自定义按钮
- QSS 收尾

### Out of Scope

- AI / Network / Replay 模块
- 音效 / i18n
- 实际障碍物地图
- QSS `rgba()` Qt5 兼容（独立 hotfix）

---

## 3. 碰撞系统目标架构

### 3.1 核心数据结构（T-11 已落地）

```cpp
enum class CollisionType { None, Wall, SelfBody, OtherBody, HeadToHead, Food, Obstacle };

struct CollisionReport {
    int snakeIndex;
    CollisionType type;
    Point position;
    int otherSnakeIndex = -1;
};
```

### 3.2 Board（class，纯空间索引）

```cpp
class Board {
public:
    struct CellInfo {
        uint8_t snakeMask = 0;
        uint8_t headMask  = 0;
        bool hasObstacle  = false;      // Food 标记由 Food::placeAt/remove 管理
    };

    Board(int w = 20, int h = 20);
    int width() const;
    int height() const;
    void setSize(int w, int h);
    void initFromSnakes(const std::vector<Snake>& snakes);

    void placeHead(int snakeIdx, Point pos);
    void placeBody(int snakeIdx, Point pos);
    void removeHead(int snakeIdx, Point pos);
    void removeTail(int snakeIdx, Point pos);

    bool isOutOfBounds(Point pos) const;
    const CellInfo& cellAt(Point pos) const;

    void setFoodFlag(Point pos, bool value);   // 仅由 Food::placeAt/remove 调用
    const std::unordered_set<Point>& freeCells() const;
    void addFreeCell(Point pos);
    void removeFreeCell(Point pos);
    const std::unordered_set<Point>& obstacles() const;
    void addObstacle(Point pos);
};
```

**设计原则**：Board 只做空间索引。食物状态归 Food class 管理，通过 `setFoodFlag` 仅设 grid 标记。

### 3.3 Food（class，食物状态集中管理）

```cpp
class Food {
public:
    Food(int points = 1);
    Point position() const;
    int points() const;
    bool isEaten() const;
    void placeAt(Point pos, Board& board);
    void remove(Board& board);
    void markEaten();
    void clearEaten();
};
```

### 3.4 GameState

```cpp
struct GameState {
    Board board;
    std::vector<Snake> snakes;
    Food food;
    int score = 0;
    bool gameOver = false;
};
```

### 3.3 Snake 变更

```cpp
// move() 读取 m_growNext 决定是否 pop 尾，但不再清除
void Snake::move() {
    m_body.insert(m_body.begin(), nextHead());
    if (m_growNext)
        /* 不 pop，不清除 */ ;
    else
        m_body.pop_back();
}

// 新增：供 MoveComponent 读后立刻清理
void clearGrowPending() { m_growNext = false; }
```

`growPending` 生命周期：
```
Tick N:   Controller 处理 Food report → snake.grow()        → m_growNext = true
Tick N+1: MoveComponent: ate = snake.growPending()           → true
          MoveComponent: snake.clearGrowPending()             → false
          MoveComponent: if (!ate) removeTail else 保留尾
```

### 3.4 MoveComponent

```cpp
void MoveComponent::update(GameState& state) {
    auto& board = state.board;
    for (size_t i = 0; i < state.snakes.size(); ++i) {
        bool ate = state.snakes[i].growPending();     // 消费前读
        state.snakes[i].clearGrowPending();            // 读后立刻清
        Point oldTail = state.snakes[i].tail();
        state.snakes[i].move();                        // 不碰 m_growNext
        Point newHead = state.snakes[i].head();

        if (!ate)
            board.removeTail(static_cast<int>(i), oldTail);
        board.placeHead(static_cast<int>(i), newHead);
    }
}
```

**只移动、只维护 grid。不检测碰撞。**

### 3.5 CollisionComponent

```cpp
class CollisionComponent {
public:
    // 纯检测——读 Board::grid，返回 CollisionReport 列表
    // 不修改 GameState，不产生副作用
    std::vector<CollisionReport> detect(const GameState& state) const;
};
```

检测逻辑（~20 行）：

```
for each snake i:
  head = snakes[i].head()
  if board.isOutOfBounds(head):
    reports += {i, Wall, head}; continue

  cell = board.cellAt(head)
  others     = cell.snakeMask & ~(1<<i)
  otherHeads = cell.headMask  & ~(1<<i)

  if otherHeads:    → {i, HeadToHead, head, firstSetBit(otherHeads)}
  else if others:   → {i, OtherBody, head, firstSetBit(others)}
  if cell.hasFood:  → {i, Food, head}
  if cell.hasObstacle: → {i, Obstacle, head}
```

**不知道游戏规则——不知道 Food 会增长、不知道 Wall 会死。**

### 3.6 Controller（游戏规则引擎）

```cpp
void GameController::update() {
    // 1. RenderComponent::syncFromState(state)
    m_render->syncFromState(m_state);

    // 2. InputComponent::apply(state)
    m_input->apply(m_state);

    // 3. MoveComponent::update(state)
    m_move->update(m_state);    // 移动 + 维护 grid

    // 4. Collision — 检测
    auto reports = m_collision->detect(m_state);

    // 5. 处理碰撞报告（游戏规则）
    bool foodEaten = false;
    for (auto& r : reports) {
        switch (r.type) {
        case Wall:
        case SelfBody:
        case OtherBody:
        case Obstacle:
            m_state.snakes[r.snakeIndex].alive = false;
            break;
        case HeadToHead:
            m_state.snakes[r.snakeIndex].alive = false;
            if (r.otherSnakeIndex >= 0)
                m_state.snakes[r.otherSnakeIndex].alive = false;
            break;
        case Food:
            if (m_state.snakes[r.snakeIndex].alive) {
                m_state.snakes[r.snakeIndex].grow();
                foodEaten = true;
            }
            break;
        }
    }
    // 判定 gameOver
    int aliveCount = 0;
    for (auto& s : m_state.snakes) if (s.alive) aliveCount++;
    if (aliveCount == 0) m_state.gameOver = true;

    if (foodEaten)
        m_state.board.markFoodEaten();

    // 6. FoodSpawner — 如果食物被吃，生成新食物
    if (m_state.board.isFoodEaten())
        m_foodSpawner->spawn(m_state);
}
```

### 3.7 FoodSpawner

```cpp
class FoodSpawner {
public:
    void spawn(GameState& state);  // 从 board.freeCells() 随机取一格，调 board.setFood()
};
```

原 `FoodComponent` 吃食物检测逻辑已迁移到 CollisionComponent（上报 Food report）+ Controller（处理 grow）。FoodComponent 重命名为 FoodSpawner。

### 3.8 5 步流水线

```
1. Render     → syncFromState(state)
2. Input      → applyDirections(state)
3. Move       → moveAll(state) + 更新 board.grid
4. Collision  → detect(state) → reports
5. Controller → 处理 reports（死亡 + 食物 + gameOver）+ FoodSpawner
```

### 3.9 删除清单

| 文件 | 原因 |
|---|---|
| `ICollisionDetector.h` | 不再需要策略接口 |
| `NaiveCollisionDetector.h/.cpp` | 位掩码替代 |
| `GridCollisionDetector.h/.cpp` | 位掩码替代 |
| `FoodComponent.h/.cpp` | 重命名为 FoodSpawner |

---

## 4. UI 变更规格

（同上版 v0.1，无变更）

### 4.1 SnakeItem：渐变 + 发光

```
paint():
  t = segmentIndex / totalLength
  color = lerp(headColor, tailColor, t)
  QPainter::setBrush(color)
  if (isHead): QGraphicsDropShadowEffect 或手动多层同心矩形模拟 glow
```

### 4.2 FoodItem：光晕

`QGraphicsDropShadowEffect` + `#ff4466`。

### 4.3 GameScene：网格线

`drawBackground()` override，纯绘制，不创建 item。

### 4.4 HUD InfoBar

字段：Score（绿）、Length（白）、Time（白）、Speed（白）、Kills（红）、Rank（绿）。

### 4.5 LeaderboardWidget

侧栏 170px：排名号、颜色圆点、玩家名、分数。自己高亮，死亡划线。

### 4.6 GameOverWidget：多人排名表

列：排名 / 玩家 / 分数 / 长度 / 击杀 / 状态。

### 4.7 ShineLabel：反光倒计时

自定义 QLabel 子类，`paintEvent()` 用 QPainter 绘制 QLinearGradient 动画。

### 4.8 SettingsWidget：Tab 重构

`QTabWidget` → QPushButton × 2 + `QStackedWidget`。

### 4.9 GamePage 布局

```
GamePage (QVBoxLayout)
  ├─ InfoBar (~50px)
  └─ QHBoxLayout
       ├─ GameView (弹性)
       └─ LeaderboardWidget (170px)
```

---

## 5. 文件变更清单

### 新增文件

```
src/app/InfoBar.h/.cpp              (T-19)
src/app/LeaderboardWidget.h/.cpp    (T-19)
src/app/ShineLabel.h/.cpp           (T-21)
src/controller/FoodSpawner.h/.cpp   (T-16)
```

### 修改文件

```
src/core/Board.h                    (class 封装 grid + 方法 — T-14)
src/core/Snake.h/.cpp               (clearGrowPending + move 不清理 — T-14)
src/controller/MoveComponent.h/.cpp (调 Board 方法 — T-15)
src/controller/CollisionComponent.h/.cpp (纯检测 detect() — T-15)
src/controller/FoodComponent → FoodSpawner (T-16)
src/controller/GameController.h/.cpp (新流水线 — T-15/T-16)
src/ui/SnakeItem.h/.cpp             (T-18)
src/ui/FoodItem.h/.cpp              (T-19)
src/ui/GameScene.h/.cpp             (T-19)
src/app/GamePage.h/.cpp             (T-20)
src/app/GameOverWidget.h/.cpp       (T-21)
src/app/SettingsWidget.h/.cpp       (T-22)
src/resources/style/main.qss        (T-23)
```

### 删除文件

```
src/controller/ICollisionDetector.h
src/controller/NaiveCollisionDetector.h/.cpp
src/controller/GridCollisionDetector.h/.cpp
```

---

## 6. 测试策略

| 测试文件 | 内容 |
|---|---|
| `test_collision_detector.cpp` | T-12/T-13 的 17 个用例适配新 CollisionComponent::detect 接口 |
| 既有测试 | MoveComponent/Snake/Board 变更适配，覆盖不减 |
| 新增测试 | 按需随 T-15/T-16 追加 |

---

## 7. 任务拆分

| ID | 标题 | 领域 | 依赖 |
|---|---|---|---|
| T-11 | 碰撞数据结构 + 接口 | Collision | — |
| T-12 | NaiveCollisionDetector + 测试 | Collision | T-11 |
| T-13 | GridCollisionDetector + 测试 | Collision | T-11 |
| **T-14** | **Board struct→class + Snake clearGrowPending** | Collision | T-13 |
| **T-15** | **MoveComponent 调 Board 方法 + CollisionComponent 纯检测** | Collision | T-14 |
| **T-16** | **FoodSpawner + Controller 流水线 + 删旧文件** | Collision | T-15 |
| T-17 | SnakeItem 渐变色 + 发光 | UI | — |
| T-18 | FoodItem 光晕 + GameScene 网格线 | UI | — |
| T-19 | InfoBar + LeaderboardWidget | UI | — |
| T-20 | GameOverWidget 多人排名表 | UI | — |
| T-21 | ShineLabel 反光文字 | UI | — |
| T-22 | SettingsWidget Tab 重构 + QSS 收尾 | UI | — |

执行顺序：**严格串行**。T-11~T-16 碰撞收尾，T-17~T-22 UI 层。

---

## 8. 风险

| 风险 | 缓解 |
|---|---|
| Board struct→class 导致既有代码编译失败 | 封装后提供 getter 替代直接字段访问 |
| 位掩码碰撞检测与旧 freeCells 判定不一致 | T-15 内 cross-validation 对比 |
| `QGraphicsDropShadowEffect` 性能不足 | 降级为 paint() 手动多层半透明矩形 |
| Qt Widgets 不支持 CSS animation | ShineLabel + QTimer + QPainter |
