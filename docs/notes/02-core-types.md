# 里程碑 2：core 基础类型

## 做什么

定义游戏模型使用的三个基础数据结构：坐标点 Point、方向枚举 Direction、食物 Food。全部 header-only。

## 怎么做

- Point：带 x/y 的二维坐标，提供默认构造函数和两参数构造函数，`operator==` 用于比较
- Direction：枚举 Up/Down/Left/Right
- Food：包含位置（Point）和分值（int），后续可扩展不同类型

## 为什么这样做

- **header-only**：这些类型没有行为逻辑，不需要 .cpp 文件，减少编译目标
- **Point 提供构造函数**：兼容 MSVC 的聚合初始化限制
- **Food 带分值字段**：为后续多种食物类型（高分/低分）预留扩展
