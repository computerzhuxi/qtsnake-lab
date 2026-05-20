# Snake Arena 架构文档

## 目录结构

```
SnakeArena/
├── assets/                 ← 素材（预留）
├── src/
│   ├── main.cpp            ← 入口：加载 QSS，创建 AppShell
│   ├── core/               ← 纯数据 + 自包含行为，不依赖 Qt
│   ├── logging/            ← 异步日志系统
│   ├── ui/                 ← QGraphicsView 渲染
│   ├── controller/         ← 游戏控制器 + 组件 + 服务（全打平）
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
| **core/** | 纯数据 (Point, Direction, Board, GameState, Food) + Snake 自身行为 | 无 |
| **logging/** | 异步日志：后台线程 + 消息队列 | 无 |
| **ui/** | QGraphicsItem 绘制，GameScene 数据→渲染 | core |
| **controller/** | Controller (循环驱动+状态机) + 6 个游戏组件 + RngService | core, ui |
| **app/** | QStackedWidget 页面管理 + 浮层 + 键盘事件分发 | controller, ui |
| **audio/** | 音效接口（空壳） | 无 |
| **resources/** | QSS + .qrc 打包 | 无 |

## 类职责速查

### core/

| 类 | 类型 | 职责 |
|----|------|------|
| `Point` | 数据 | 二维坐标 (x, y)，含 operator== / != / hash |
| `Direction` | 枚举 | {Up, Down, Left, Right} |
| `Board` | 数据+索引 | 宽/高、位掩码网格 (grid)、空闲格集 (freeCells)、障碍物 |
| `GameState` | 数据聚合 | Board + vector\<Snake\> + Food + score + gameOver |
| `Snake` | 数据+行为 | 身体段、move()、grow()、setDirection()、growPending()（读即消费） |
| `Food` | 数据+行为 | 位置、分值、被吃状态、placeAt(board)/remove(board) |
| `CollisionReport` | 数据 | CollisionType 枚举 + 碰撞事件结构 |

### controller/

| 类 | 职责 |
|----|------|
| `GameController` | QTimer + 状态机 (Idle/Ready/Countdown/Playing/Paused/GameOver)，6 步流水线，processCollisions() 处理碰撞响应 |
| `RngService` | 集中随机数（mt19937 + 种子注入），仅供 controller 组件使用 |
| `InputComponent` | 键盘方向缓存 → snake.setDirection() |
| `MoveComponent` | snake.move() + 增量维护 Board::grid（placeHead/removeTail） |
| `CollisionComponent` | detect(state) const → vector\<CollisionReport\>，读 Board::grid 位掩码，纯检测 |
| `FoodSpawner` | 从 Board::freeCells 随机生成食物，调 food.placeAt(board) |
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
  3. MoveComponent      ← 蛇移动+增量更新 Board::grid
  4. CollisionComponent ← detect(state) → reports（读 grid 位掩码）
  5. processCollisions  ← 死亡/食物/gameOver 响应
  6. FoodSpawner        ← 如果 foodEaten: spawn(state)
  ↓                                   │
GameScene::syncFromState()            │
  ├─ SnakeItem::setPos()              │
  └─ FoodItem::setPos()              │
```

## 位掩码碰撞逻辑

```
蛇移动后 (MoveComponent):
  旧尾 → board.removeTail(i, oldTail)  // snakeMask 清位，snakeMask==0 时 insert freeCells
  新头 → board.placeHead(i, newHead)   // snakeMask+headMask 置位，freeCells erase

碰撞检测 (CollisionComponent::detect):
  读 head 所在 CellInfo:
    others     = cell.snakeMask & ~(1<<i)   // 除了自己，有别的身体吗
    otherHeads = cell.headMask  & ~(1<<i)   // 除了自己，有别的头吗
    otherHeads → HeadToHead
    others     → OtherBody
    head == food.position() → Food
    cell.hasObstacle → Obstacle
    head 出界 → Wall
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

- **State 数据架构**：Board/GameState/Food 纯数据，Controller 逻辑，联机/回放直接存 State 快照
- **组件化 Controller**：6 个独立组件 (Input/Move/Collision/FoodSpawner/Render) + RngService，unique_ptr 持有
- **位掩码碰撞**：Board::grid 每格 2 字节（snakeMask + headMask），MoveComponent 增量维护，CollisionComponent::detect 位运算 O(1) 查碰撞
- **碰撞/响应分离**：CollisionComponent 纯检测（返回 reports），Controller::processCollisions 处理响应（先死后吃）
- **死亡帧策略**：`RenderComponent` 在 `update()` 序列首位执行，渲染的是"上一 tick 结束态"。当某帧发生碰撞，该帧不会再被渲染一次，玩家屏幕停留在撞墙前的最后合法位置。
- **Qt5 Widgets + QGraphicsView**：成熟稳定
- **QSS 全局样式**：暗色电竞风
- **异步日志**：后台线程 + 消息队列
- **增量更新渲染**：蛇增长才 new，不增长只 setPos
- **QStackedWidget 页面分离**：主菜单和游戏独立页，enter/exit 生命周期
