# 里程碑 3：Snake 蛇模型 (TDD)

## 做什么

用测试驱动开发实现 Snake 类的全部行为：初始状态、移动、方向控制、增长。

## 怎么做

先写 12 个测试，再实现 Snake.cpp：

1. `initialLengthIsThree` — 新蛇有 3 节身体
2. `initialDirectionIsRight` — 默认方向向右
3. `customInitialDirection` — 支持自定义初始方向
4. `bodyOppositeToDirection` — 身体段往方向的反方向生成（如方向 Right，身体在左边）
5. `initialHeadPosition` — 头部在指定起始位置
6. `moveUpdatesPositions` — 移动后头部位置更新
7. `moveUp` / `moveLeft` / `moveDown` — 四个方向移动
8. `bodyFollowsHeadAfterMoves` — 身体跟随头部
9. `cannotReverseToOppositeDirection` — 禁止 180 度反转
10. `growIncreasesLength` — grow() 后长度 +1

Snake 实现：
- 构造函数 `Snake(Point start, Direction dir)`，头在 start，身体往 dir 的反方向延伸
- 用 `std::vector<Point>` 存身体，`m_body[0]` 是头部
- move() 在头部插入新位置，尾部 pop_back（除非 growNext 标记）
- setDirection() 检查不与当前方向相反
- grow() 设置标记，下次 move 不删尾

注：Snake 不做边界检查。随机出生位置和防越界/防重叠由 GameController (Task 8) 负责。

## 为什么这样做

- **TDD 先行**：每个行为有测试约束，防止回归
- **禁止反转**：防止玩家快速按反向键自杀，提高操作容错
- **自定义初始方向**：联机/AI 多蛇时每条蛇可从不同侧出生面对不同方向
- **职责分离**：Snake 只管自身，边界/碰撞/出生点分配由 Board 和 Controller 负责
