# Snake Arena 架构文档

## 目录结构

```
SnakeArena/
├── assets/                 ← 素材（预留）
├── src/
│   ├── main.cpp            ← 入口：加载 QSS，创建 AppShell
│   ├── core/               ← 纯数据 + Snake 自身行为，不依赖 Qt
│   ├── logging/            ← 异步日志系统
│   ├── ui/                 ← QGraphicsView 渲染
│   ├── controller/         ← 游戏控制器 + 5 个组件
│   │   └── components/
│   ├── app/                ← 应用外壳（页面管理、浮层）
│   ├── audio/              ← 音效接口（空壳）
│   └── resources/          ← QSS + .qrc
├── tests/
├── docs/
└── CMakeLists.txt
```

## 模块职责

| 模块 | 职责 | 依赖 |
|------|------|------|
| **core/** | 纯数据 (Point, Direction, Board, GameState) + Snake 自身行为 | 无 |
| **logging/** | 异步日志：后台线程 + 消息队列 | 无 |
| **ui/** | QGraphicsItem 绘制，GameScene 数据→渲染 | core |
| **controller/** | Controller (循环驱动+状态机) + 5 个游戏组件 | core, ui |
| **app/** | QStackedWidget 页面管理 + 浮层 + 键盘事件分发 | controller, ui |
| **audio/** | 音效接口（空壳） | 无 |
| **resources/** | QSS + .qrc 打包 | 无 |

## 类职责速查

### core/

| 类 | 类型 | 职责 |
|----|------|------|
| `Point` | 数据 | 二维坐标 (x, y)，含 operator== / != / hash |
| `Direction` | 枚举 | {Up, Down, Left, Right} |
| `Board` | 纯数据 | 宽/高、空闲格集 (freeCells)、食物位置 (foodPos) |
| `GameState` | 数据聚合 | Board + vector\<Snake\> + gameOver + score |
| `Snake` | 数据+行为 | 身体段、move()、grow()、setDirection()、growPending() |

### controller/

| 类 | 职责 |
|----|------|
| `GameController` | QTimer + 状态机 (Idle/Ready/Countdown/Playing/Paused/GameOver)，按序调度组件 |
| `InputComponent` | 键盘方向 → 蛇方向，init() 从蛇同步初始方向 |
| `MoveComponent` | snake.move() + 空闲格维护（旧尾 insert，新头留给 Collision） |
| `CollisionComponent` | 基于空闲集碰撞检测 + 头-头碰撞 + 安全占领 |
| `FoodComponent` | 初始生成食物 + 吃检测 + 从空闲集生成新食物 |
| `RenderComponent` | GameState → GameScene::syncFromState() |

### ui/

| 类 | 职责 |
|----|------|
| `SnakeItem` | QGraphicsItem：方块蛇 + 霓虹绿 |
| `FoodItem` | QGraphicsItem：圆形食物 + 红色光晕 |
| `GameScene` | QGraphicsScene：增量更新渲染，GameState → QGraphicsItem |
| `GameView` | QGraphicsView：黑底、抗锯齿、无边框 |

### app/

| 类 | 职责 |
|----|------|
| `AppShell` | 顶层窗口：QStackedWidget，键盘事件分发 |
| `GamePage` | 游戏页面：GameView 底层 + 浮层，enter/exit 生命周期 |
| `MainMenuWidget` | 主菜单：标题 + 模式按钮 + 设置/退出 |
| `PauseWidget` | 暂停浮层 |
| `GameOverWidget` | 结算浮层：分数 + 操作按钮 |
| `SettingsWidget` | 设置浮层：键位/语言/速度/大小/音量 |

### audio/

| 类 | 职责 |
|----|------|
| `AudioManager` | 单例音效接口（空壳） |
| `SoundEffect` | 枚举 {Eat, Die, MenuClick, Countdown, Go} |

## 数据流

```
AppShell::keyPressEvent()
  ↓ InputComponent::setDirection()
InputComponent (存储方向)

每 100ms (QTimer tick) ──────────────┐
                                      │
Controller::update()                  │
  1. RenderComponent    ← 渲染上一帧  │
  2. InputComponent     ← 方向写入蛇  │
  3. MoveComponent      ← 蛇移动      │
  4. CollisionComponent ← 空闲集碰撞  │
  5. FoodComponent      ← 吃+生成     │
  ↓                                   │
GameScene::syncFromState()            │
  ├─ SnakeItem::setPos()              │
  └─ FoodItem::setPos()              │
```

## 空闲集碰撞逻辑

```
蛇移动后:
  旧尾 → insert 回空闲集（释放）
  新头 → 暂不移除（留给 Collision）

碰撞检测:
  head 在空闲集？  → 安全，erase 占领
  head 在空闲集且与其他头重合？ → 死亡
  head 不在空闲集且不是食物？ → 死亡（墙/身体）
```

## 状态机

```
Idle → Ready → Countdown → Playing → GameOver
                 ↑              ↓
                 └─ (倒计时结束) → Paused
                                   ↓
                                 Playing (resume)
```

## 构建产物

| Target | 包含模块 | 
|--------|---------|
| `SnakeArena.exe` | core + ui + controller + app + logging + audio + resources |
| `SnakeServer.exe` | core + network + logging（阶段 3） |

## 关键设计决策

- **State 数据架构**：Board/GameState 纯数据，Controller 逻辑，联机/回放直接存 State 快照
- **组件化 Controller**：5 个独立组件 (Input/Move/Collision/Food/Render)，unique_ptr 持有
- **空闲集统一碰撞**：撞墙/自撞/撞他蛇 统一用 freeCells 判断，无需 Snake::checkSelfCollision
- **死亡帧策略**：`RenderComponent` 在 `update()` 序列首位执行，渲染的是"上一 tick 结束态"。当某帧发生碰撞（`MoveComponent` 已把头推入非法格、`CollisionComponent` 标记 `gameOver`），该帧不会再被渲染一次，因此玩家屏幕停留在撞墙前的最后合法位置。这是有意为之的 UX，避免显示蛇头与墙体重合的画面。
- **Qt5 Widgets + QGraphicsView**：成熟稳定
- **QSS 全局样式**：暗色电竞风
- **异步日志**：后台线程 + 消息队列
- **增量更新渲染**：蛇增长才 new，不增长只 setPos
- **QStackedWidget 页面分离**：主菜单和游戏独立页，enter/exit 生命周期
