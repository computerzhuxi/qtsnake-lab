# T-07 — QSS 集中化（清除内联样式）

> Phase-2 · 改动幅度：小 · 串行序号：7

## 任务描述

把 `GamePage::showCountdown` 中内联的 `setStyleSheet` 迁移到全局 `main.qss`，通过 `objectName` + 动态属性选择器实现三态切换（提示文字 / 数字 / GO!）。

## 输入约束

- 可修改文件，仅限：
  - `src/app/GamePage.cpp`
  - `src/app/GamePage.h`（仅必要时，例如改控件属性名）
  - `src/resources/style/main.qss`
- **不得修改其他文件。**
- 不得新增控件类。

## 输出约束

### 控件侧

- 给 `m_countdownLabel` 设置：
  - `setObjectName("CountdownLabel")`
  - 通过动态属性区分三态：
    - `setProperty("state", "hint")` 用于"按任意键开始"
    - `setProperty("state", "count")` 用于 3/2/1 数字
    - `setProperty("state", "go")` 用于 "GO!"
  - 每次切换状态后，必须 `m_countdownLabel->style()->unpolish(m_countdownLabel); m_countdownLabel->style()->polish(m_countdownLabel);` 触发 QSS 重新应用。
- `showCountdown` **仅改文字、对齐、`state` 属性**，**不再调用 `setStyleSheet`**。

### QSS 侧

- 在 `main.qss` 新增（**字面措辞自定，但选择器与属性名必须严格匹配以上约定**）：

```css
QLabel#CountdownLabel { background: transparent; }
QLabel#CountdownLabel[state="hint"] { font-size: 18px; color: #556; padding-bottom: 60px; }
QLabel#CountdownLabel[state="count"] { font-size: 72px; color: #00ff88; }
QLabel#CountdownLabel[state="go"] { font-size: 72px; color: #00ff88; }
```

## 验收标准

- `git grep -n "setStyleSheet" src/app/` 命中数为 **0**。
- 手工冒烟：进入单人模式 → 出现"按任意键开始" → 任意键 → 3 / 2 / 1 / GO! 显示效果与改造前 **视觉一致**（截图对比）。
- 编译通过；既有测试不受影响。

## 不允许做的事

- 不允许动 `src/resources/resources.qrc`。
- 不允许把样式塞进 C++ 字符串常量。
- 不允许引入 QML / QtQuick。

## 反问提示

- 若发现既有 QSS 已有某个全局 `QLabel` 通用规则与本卡冲突，**反问主程**是否调整选择器优先级。
