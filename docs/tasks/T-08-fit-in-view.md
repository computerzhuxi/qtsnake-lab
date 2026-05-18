# T-08 — 视图自适应（resize fitInView）

> Phase-2 · 改动幅度：极小 · 串行序号：8

## 任务描述

窗口缩放后游戏视图保持等比例（棋盘永远是正方形 / 等比矩形）。

## 输入约束

- 可修改文件，仅限：
  - `src/app/GamePage.h`
  - `src/app/GamePage.cpp`
- **不得修改 `GameView` / `GameScene`。**
- 不得修改其他模块。

## 输出约束

- `GamePage::resizeEvent(QResizeEvent*)` 末尾追加：
  - 条件：`m_scene != nullptr && !m_scene->sceneRect().isEmpty()`
  - 行为：`m_view->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);`

## 验收标准

- 手工冒烟：
  - 进入单人模式 → 拖动窗口边缘横向拉宽 → 棋盘仍保持原比例（不变形）。
  - 横向、纵向、最大化、还原各 1 次，棋盘始终居中等比。
- 编译通过；既有测试通过。

## 不允许做的事

- 不允许重写 `fitInView` 逻辑或缓存 transform。
- 不允许给 `GameView` 加方法。

## 反问提示

- 若发现 `sceneRect()` 在场景为空时返回奇异值（例如 0×0），导致 `fitInView` 触发 Qt 警告，**反问主程**是否在 `m_scene->clearAll()` 之外保留最小占位 rect。
