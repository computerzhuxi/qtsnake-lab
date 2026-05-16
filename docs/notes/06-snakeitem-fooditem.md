# 里程碑 6：SnakeItem + FoodItem 渲染

## 做什么

用 QGraphicsItem 子类渲染蛇身方块和食物圆形，暗色电竞风格。

## 怎么做

**SnakeItem：**
- 方块 20x20 + 4px 圆角
- 头部：填充 `#00ff88` + 边框 `#44ffaa`
- 身体：填充 `#00bb55` + 边框 `#22cc66`
- head/body 在 24x24 的 cell 内居中
- 通过 `m_isHead` 区分头/身

**FoodItem：**
- 圆形，半径 6px
- 填充 `#ff4466`，无边线
- 24x24 cell 内居中

两个类都重写 `boundingRect()` 和 `paint()`，不需要信号槽。

## 为什么这样做

- **方块蛇 + 圆食物**：设计阶段确定的视觉方案
- **QGraphicsItem 而非手动 draw**：Qt 自动处理坐标变换和视口缩放
- **头身颜色差异**：玩家一眼看出蛇的朝向
- **圆角**：比直角矩形更现代化，符合电竞主题
