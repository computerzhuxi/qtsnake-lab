# 里程碑 5：controller 游戏控制器

## 做什么

实现游戏主循环和状态机，连接键盘输入 → Snake 方向 → Board 更新 → Scene 渲染的完整链路。

## 怎么做

- QTimer 驱动 tick()（100ms 间隔），每次 tick 执行：移动蛇 → 碰撞检测 → 吃食物判断 → 更新 Scene
- 状态机：Idle → Ready（等待按键）→ Countdown（3-2-1-GO）→ Playing → Paused / GameOver
- 倒计时用嵌套 QTimer，每秒触发一次，到达 GO 后切换到 Playing
- handleKeyPress() 仅在 Playing 状态下生效，ESC 在 Playing/Paused 间切换

## 为什么这样做

- **QTimer 而非独立线程**：Qt 事件循环天然融合，100ms 间隔对贪吃蛇足够，无需线程同步
- **状态机显式管理**：每种状态的行为边界清晰——Ready 只能触发倒计时，Playing 才处理方向键
- **倒计时独立于游戏 tick**：用 700ms 间隔的单独 timer，倒计时结束后启动主循环
