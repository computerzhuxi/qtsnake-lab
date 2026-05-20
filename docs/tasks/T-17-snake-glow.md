# T-17：SnakeItem 渐变色 + 霓虹发光

> 签发：主程 | 日期：2026-05-19 | 状态：待领取
> 分支：`phase-3-collision-refactor`

---

## 任务描述

重写 `SnakeItem::paint()`：蛇身从头到尾逐段渐变色 + 蛇头霓虹发光效果。

---

## 输入约束

### 允许修改的文件

```
~ src/ui/SnakeItem.h   (可能需要加 t 参数 / totalLength)
~ src/ui/SnakeItem.cpp (paint 重写)
~ src/ui/GameScene.cpp (传入段索引/总长度给 SnakeItem)
```

### 不允许做的事

- 不得修改 Board / Snake / Controller 等非 UI 文件
- 不得修改 `SnakeItem::boundingRect()` / `cellSize` / `blockSize`
- 不得引入第三方依赖
- 不得在 `paint()` 中分配堆内存

---

## 输出约束

### 1. 渐变色

每段颜色在头尾色之间按位置插值：

```
头色: #00ff88 (0, 255, 136)
尾色: #00bb55 (0, 187, 85)

t = segmentIndex / (totalLength - 1)   // 0.0 (头) ~ 1.0 (尾)
r = 0
g = 255 + t * (187 - 255) = 255 - 68*t
b = 136 + t * (85 - 136)  = 136 - 51*t
```

逐段颜色不同，不是头/身两色硬切换。

### 2. 霓虹发光（蛇头）

方案：在 `paint()` 中用半透明 QPen 画多层同心矩形，逐层放大透明度递减。

```cpp
if (m_isHead) {
    // 发光层（由外向内，逐渐不透明）
    for (int layer = 3; layer >= 0; --layer) {
        int alpha = 40 - layer * 10;   // 40, 30, 20, 10
        int offset = (layer + 1) * 2;  // 8, 6, 4, 2
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(0, 255, 136, alpha));
        painter->drawRoundedRect(QRectF(-bs/2 - offset, -bs/2 - offset,
                                        bs + offset*2, bs + offset*2), r+1, r+1);
    }
    // 主体
    painter->setPen(QPen(QColor("#44ffaa"), 1));
    painter->setBrush(m_color);
} else {
    painter->setPen(QPen(QColor("#22cc66"), 1));
    painter->setBrush(m_color);
}
painter->drawRoundedRect(QRectF(-bs/2, -bs/2, bs, bs), r, r);
```

如果 `QGraphicsDropShadowEffect` 效果更好且性能可接受，优先用它——代码更少。两者都试，取效果好的一方。

### 3. segmentIndex / totalLength 传递

`SnakeItem` 需要知道当前段在蛇身中的位置。方案：

- `SnakeItem` 加 `void setSegmentIndex(int index, int total)` 方法
- `GameScene::syncFromState()` 中每段调用

或简化：SnakeItem 存 `m_color`，由 GameScene 计算好颜色后传入。推荐后者——SnakeItem 不管颜色计算逻辑。

```cpp
// SnakeItem 新增
void setColor(const QColor& color) { m_color = color; update(); }

// GameScene 中计算
int total = body.size();
for (int j = 0; j < total; ++j) {
    float t = total > 1 ? float(j) / (total - 1) : 0.0f;
    int g = 255 - int(68 * t);
    int b = 136 - int(51 * t);
    items[j]->setColor(QColor(0, g, b));
    items[j]->setPos(...);
    items[j]->setIsHead(j == 0);
}
```

---

## 验收标准

1. 编译 /W4 零警告
2. 45 测试全绿
3. 蛇身从头到尾颜色连续渐变（非两色硬切换）
4. 蛇头有霓虹发光效果（肉眼可见的辉光）
5. 性能：单蛇 20 段以下无明显卡顿
6. 手工冒烟正常

---

## 提交格式

```
改动文件清单:
~ src/ui/SnakeItem.h
~ src/ui/SnakeItem.cpp
~ src/ui/GameScene.cpp

一句冒烟:
编译 /W4 零警告，45 测试全绿，SnakeArena.exe 蛇身渐变+发光正常。
```
