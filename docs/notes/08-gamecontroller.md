# 里程碑 8：GameController

## 做什么

实现游戏控制器：QTimer 驱动主循环、状态机管理、输入处理、碰撞检测整合。

## 数据流

```
键盘方向键
    ↓
GameController::handleKeyPress(Direction)
    ↓ setDirection()
Snake::m_direction

QTimer::tick() (100ms)
    ↓
GameController::tick()
    ├─ Snake::move()
    ├─ Board::checkWallCollision() / Snake::checkSelfCollision()
    ├─ Board::checkFoodCollision() → Snake::grow() + Board::spawnFood()
    └─ GameScene::syncFromBoard()
```

## 怎么做

**状态机：**
```
Idle → Ready → Countdown → Playing → Paused
                     ↓           ↑        ↓
                  GameOver ← ← ← ← ← ← ← ┘
```

- `startGame()`：创建 Board + Snake，全棋盘随机出生，再根据位置选安全方向
- `handleReadyKey()`：Ready → Countdown，3-2-1-GO 倒计时链式 QTimer
- `tick()`：Playing 态每 100ms 执行移动→碰撞→食物→渲染
- `pause()`/`resume()`：暂停/恢复 QTimer
- `reset()`：清理，回到 Idle

**随机出生：**
- 在棋盘内部（距边界 >= 2）随机选位置
- 根据位置智能选方向：离哪边近就排除哪个方向，保证初始 3 节身体不越界

**信号：**
- `stateChanged(State)` → AppShell 控制浮层显隐
- `countdownTick(number)` → 倒计时显示
- `scoreChanged(score)` → 分数更新

## 为什么这样做

- **状态机显式管理**：每种状态行为边界清晰，Playing 才处理方向键，Ready 只能触发倒计时
- **QTimer 而非独立线程**：Qt 事件循环天然融合，无需线程同步
- **随机出生+智能方向**：保证初始身体不越界，重叠问题由联机模块处理
