# 里程碑 2：core 游戏模型

## 做什么

实现贪吃蛇核心数据模型：Point 坐标、Direction 方向枚举、Snake 蛇模型、Board 棋盘模型。纯 C++，不依赖 Qt。

## 怎么做

- Point：带 x/y 的二维坐标，提供构造函数和 `operator==`
- Direction：枚举 Up/Down/Left/Right
- Snake：用 `std::vector<Point>` 存身体段，头部在 front。move() 时头部插入新位置、尾部弹出（除非 growNext）。setDirection() 禁止 180 度反转。grow() 标记下次 move 不删尾
- Board：管理棋盘尺寸、std::mt19937 随机数引擎（支持种子注入保证回放确定性）、三种碰撞检测（撞墙/撞自己/吃食物）、食物生成

## 为什么这样做

- **TDD 先行**：先写 17 个测试再实现，保证每个行为有规格约束
- **种子可控的随机数**：mt19937 + 可选种子，回放时可以复现完全相同的食物序列
- **Snake 用 vector 存 body**：插入头部 O(n)，但对蛇的长度（一般 < 100）来说足够快，实现简单直观
- **禁止反转而非忽略**：防止玩家快速按反向键自杀，提高操作容错
