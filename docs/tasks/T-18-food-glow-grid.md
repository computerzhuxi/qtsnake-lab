# T-18：FoodItem 光晕 + GameScene 网格线

> 签发：主程 | 日期：2026-05-19 | 状态：待领取
> 分支：`phase-3-collision-refactor`

---

## 任务描述

1. **FoodItem 光晕**：食物圆形外围加红色光晕，与蛇头发光风格一致。
2. **GameScene 网格线**：棋盘上绘制微弱分隔线，增强空间感。

---

## 输入约束

### 允许修改的文件

```
~ src/ui/FoodItem.h
~ src/ui/FoodItem.cpp      (paint 加光晕)
~ src/ui/GameScene.h        (如 grid 线需要缓存)
~ src/ui/GameScene.cpp      (drawBackground / 网格绘制)
```

### 不允许做的事

- 不得修改 SnakeItem / Board / Controller 等
- 不得在 paint 中分配堆内存
- 不得修改 FoodItem::boundingRect / cellSize / radius

---

## 输出约束

### 1. FoodItem 光晕

与 T-17 蛇头发光同风格：`paint()` 中用半透明 QBrush 画多层同心圆。

```cpp
// 光晕：3 层（由外向内）
for (int layer = 2; layer >= 0; --layer) {
    int alpha = 35 - layer * 12;           // 35, 23, 11
    int glowR = radius + (layer + 1) * 3;  // 15, 12, 9
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(0xff, 0x44, 0x66, alpha));
    painter->drawEllipse(QPointF(0, 0), glowR, glowR);
}
// 主体
painter->setBrush(QColor("#ff4466"));
painter->drawEllipse(QPointF(0, 0), radius, radius);
```

`boundingRect` 需要扩展以容纳光晕（+9px）。

### 2. GameScene 网格线

在 `drawBackground()` 中绘制——不创建额外 QGraphicsItem，纯 QPainter 调用。

```cpp
void GameScene::drawBackground(QPainter* painter, const QRectF& rect) {
    QGraphicsScene::drawBackground(painter, rect);  // 先画黑底

    painter->setPen(QPen(QColor(255, 255, 255, 8), 0.5));  // rgba(255,255,255,0.03)
    int cellSize = SnakeItem::cellSize;
    int w = m_boardW * cellSize;
    int h = m_boardH * cellSize;

    for (int x = 0; x <= m_boardW; ++x)
        painter->drawLine(x * cellSize, 0, x * cellSize, h);
    for (int y = 0; y <= m_boardH; ++y)
        painter->drawLine(0, y * cellSize, w, y * cellSize);
}
```

`m_boardW` / `m_boardH` 已有（在 `syncFromState()` 中维护）。

---

## 验收标准

1. 编译 /W4 零警告
2. 45 测试全绿
3. 食物有可见红色光晕（与蛇头发光风格匹配）
4. 棋盘上有微弱网格线
5. 性能无明显退化
6. 手工冒烟正常

---

## 提交格式

```
改动文件清单:
~ src/ui/FoodItem.h
~ src/ui/FoodItem.cpp
~ src/ui/GameScene.cpp

一句冒烟:
编译 /W4 零警告，45 测试全绿，SnakeArena.exe 食物光晕+网格线正常。
```
