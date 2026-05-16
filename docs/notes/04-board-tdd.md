# 里程碑 4：Board 棋盘模型 (TDD)

## 做什么

测试驱动实现 Board 类：棋盘尺寸管理、墙壁碰撞和食物碰撞检测、随机食物生成（支持种子注入保证回放确定性）。自撞检测归 Snake 管理。

## 怎么做

先写 7 个测试，再实现 Board.cpp：

1. `initializesWithGivenSize` — 棋盘尺寸正确
2. `snakeWithinBoundsIsAlive` — 蛇在范围内不撞墙
3. `snakeHitsLeftWall` — 蛇撞左墙检测
4. `foodPlacedWithinBoard` — 食物在棋盘范围内
5. `foodNotOnSnakeBody` — 食物不生成在蛇身上
6. `checkFoodCollisionWhenHeadOnFood` — 蛇头碰到食物
7. `seedProducesSameFood` — 相同种子产生相同食物序列

Board 实现：
- `Board(width, height, seed)`：种子为 0 时用 random_device，否则用指定值
- `spawnFood(const Snake* snakeToAvoid)`：随机生成食物位置，避开蛇身
- `checkWallCollision(snake)`：蛇头坐标 < 0 或 >= 宽高
- `checkFoodCollision(snake)`：蛇头位置 == 食物位置
- `setSeed(seed)`：运行时更换种子（回放用）

## 为什么这样做

- **种子可控的 mt19937**：回放时用相同种子再现完全相同的食物序列
- **spawnFood 接受蛇参数**：避免食物生成在蛇身上，同时保持调用方简洁
- **自撞检测归 Snake**：蛇持有自己的身体数据，自撞检测是蛇的内部行为。Board 只管棋盘相关（墙壁、食物）。AI 寻路时从 Controller 直接拿所有蛇身体拼全局占用图，不需要 Board 参与
