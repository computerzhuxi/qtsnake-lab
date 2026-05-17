# 里程碑 7：GameScene + GameView

## 做什么

GameScene 承担 core → ui 的数据对接：将 Board 和 Snake 的模型数据转换为 QGraphicsItem 画面。采用增量更新避免每帧重建对象。GameView 配置渲染视口。

## 数据流

```
GameController::tick()
    │
    └─ GameScene::syncFromBoard(board, snakes)
           ├─ 蛇段 item 数量 < 身体长度 → new SnakeItem (增长)
           ├─ 蛇段 item 数量 > 身体长度 → delete 多余 item (其他蛇死了)
           ├─ 遍历每条蛇的身体 → 已有 item 调 setPos() + setIsHead() 更新
           └─ FoodItem 首次 new，后续只调 setPos()
```

## 怎么做

**SnakeItem 新增：**
- `setIsHead(bool)`：切换头/身外观，复用已有 item

**GameScene：**
- `syncFromBoard(board, snakes)`：增量更新
  - 蛇段够了只调 `setPos()` + `setIsHead()`
  - 蛇增长了才 `new SnakeItem()`
  - 蛇减少了删除多余 item
- 食物首次创建后只调 `setPos()`，不反复 new/delete
- `cellToPixel(cell)`：`cell * 24 + 12`
- `clearAll()`：重置时清空所有 item

**GameView：**
- 抗锯齿 + 隐藏滚动条 + 黑背景 `#06060f` + 全视口刷新 + 无边框

## 为什么这样做

- **增量更新而非全量刷新**：每帧只调 `setPos()`，避免重复 new/delete，面试展示性能意识
- **按需增长**：蛇 body 变长才 new，不会在 10 FPS 下每帧创建几十个临时对象
- **cellToPixel 统一在 GameScene**：修改格子大小时只改一处
