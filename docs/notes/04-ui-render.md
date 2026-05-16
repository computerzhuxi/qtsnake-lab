# 里程碑 4：ui 游戏渲染

## 做什么

用 QGraphicsView/QGraphicsScene/QGraphicsItem 将 Board 模型数据渲染为暗色电竞风格的画面。

## 怎么做

- SnakeItem：QGraphicsItem 子类，方块造型（20x20）+ 4px 圆角 + 霓虹绿填充，头部颜色更亮用于区分
- FoodItem：QGraphicsItem 子类，圆形（半径 6px）+ 红色 `#ff4466`
- GameScene：QGraphicsScene 子类，`syncFromBoard()` 方法清空旧图形、按 Board 数据重建所有 SnakeItem 和 FoodItem
- GameView：QGraphicsView 子类，配置抗锯齿、隐藏滚动条、黑色背景 `#06060f`、全视口刷新模式

## 为什么这样做

- **每帧全量刷新而非增量更新**：Qt 场景项数量不大（蛇 + 食物），全量重建比逐个维护位置的代码简单。FullViewportUpdate 模式配合此策略
- **蛇方块 + 食物圆形**：设计阶段确认的视觉效果，从核心模型到渲染层保持一致
- **cellToPixel 坐标转换**：棋盘坐标（0-19）→ 像素坐标，统一在 Scene 层处理
