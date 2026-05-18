# 里程碑 12：重构 — State 数据架构

## 做什么

将 Board 转为纯数据层，新增 GameState 聚合数据，Controller 集中所有规则逻辑。

## 新结构

```
Snake      数据类：身体、方向、自撞
Board      纯数据：地图尺寸、空闲格列表、食物位置
GameState  数据聚合：Board + vector<Snake> + gameOver 标志
Controller 规则引擎：持有 GameState + QTimer，所有逻辑集中
GameScene  渲染：直接从 GameState 读数据
```

## 改动清单

**core/**：
- `Point.h` — 加 `std::hash<Point>` 特化
- `Board.h` — 改为 struct：width/height/freeCells/food
- `GameState.h` — 新增：聚合 Board + Snakes + gameOver 标志
- `Snake.h` — 加默认构造 + `growPending()` 扩展

**controller/**：
- `GameController` — 重写 tick()，操作 GameState

**ui/**：
- `GameScene` — syncFromState(GameState) 替代 syncFromBoard

**app/**：
- `AppShell` — 适配新接口

## 为什么这样做

- Board 是数据 → 回放时直接存就是快照
- GameState 是数据 → 联机时就是同步包
- Controller 是逻辑 → 规则集中一处，改规则不动数据
- 测试可手工构造任意 State 场景
