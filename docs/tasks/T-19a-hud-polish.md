# T-19a：InfoBar / LeaderboardWidget 视觉对齐 HTML 原型 + 布局 bug 修复

> 签发：主程 | 日期：2026-05-21 | 状态：待领取
> 分支：`phase-3-collision-refactor`
> 前置卡：T-19（InfoBar + LeaderboardWidget 骨架已搭建，存在布局 bug）

---

## 任务描述

将 T-19 已搭建的 InfoBar、LeaderboardWidget、GamePage 布局修至与 `docs/prototypes/ui-mockup-v4.html` 视觉一致，并修复 InfoBar 内容缩在左上角的布局 bug。

---

## 输入约束

### 允许修改/新增的文件

```
~ src/app/InfoBar.cpp              (列对齐方式 + 如果布局 bug 根因在此)
~ src/app/GamePage.cpp             (如果布局 bug 根因在此)
~ src/app/LeaderboardWidget.cpp    (如果布局 bug 根因在此)
~ src/resources/style/main.qss     (新增底部/左侧边框样式)
```

### 不允许做的事

- 不得修改 core / controller / ui 层任何文件
- 不得在 `src/app/` 中使用 `setStyleSheet`
- 不得修改 InfoBar.h / LeaderboardWidget.h / GamePage.h 的公开接口
- 不得使用 `using namespace std;`
- 不得修改 GamePage 布局结构（InfoBar 顶 + HBox(GameView + Leaderboard) 的骨架不变）
- 不得为了"修复"而绕过 QSS 走内联样式

---

## 输出约束

### 1. InfoBar 列对齐：从左对齐改为居中聚拢（匹配 HTML）

**现状**（T-19 Task Card 原写法）：
```
[Score列] Sep [Length列] Sep [Time列] Sep [Speed列] Sep [Kills列] Sep [Rank列] Stretch
→ 列组紧贴左侧，右侧一大片空白
```

**目标**（HTML `.hud { justify-content: center; gap: 20px; }`）：
```
Stretch [Score列] gap [Length列] gap [Time列] gap [Speed列] gap [Kills列] gap [Rank列] Stretch
→ 列组居中聚拢，两侧等距留白
```

改动点：InfoBar.cpp 构造中，在第一个 `addStat` 之前插入 `row->addStretch()`，保留末尾 `addStretch()`。删除 spacing=0 的设置，改为 `row->setSpacing(20)` 模拟 HTML 的 gap。

**注意**：gap 用 layout spacing 实现即可，不需要每个列之间额外 spacer。HTML 的 20px gap 在 Qt 中对应 `QHBoxLayout::setSpacing(20)`。

### 2. InfoBar 底部发光边框（QSS）

HTML：`border-bottom: 1px solid rgba(0,255,136,0.12)`

Qt5 不支持 `rgba()`。替代方案——用 `qlineargradient` 水平渐变模拟一根 1px 高的亮线：

```css
QWidget#infoBar {
    background: #0a0a1a;
    border-bottom: 1px solid #0f2f1a;
}
```

`#0f2f1a` = 约 #00ff88 在 8-10% 不透明度叠加在 #0a0a1a 上的近似色。视觉效果和 HTML 原型足够接近。

> 注：如果后续 Qt5 rgba hotfix 落地，可以直接替换为 `rgba(0,255,136,30)` 更精确。

### 3. LeaderboardWidget 左侧边框（QSS）

HTML：`border-left: 1px solid rgba(0,255,136,0.10)`

同样降级为近似实色：

```css
QWidget#leaderboard {
    background: #08081a;
    border-left: 1px solid #0d2a17;
}
```

`#0d2a17` = 约 #00ff88 在 8% 不透明度叠加在 #08081a 上的近似色。

### 4. 布局 bug 调试：InfoBar 内容缩在左上角

**现象**：InfoBar 内部的六列内容挤在 widget 左上角，未横向撑满。

**已知信息**：
- `setSizePolicy(Expanding, Fixed)` 已设，非根因
- 开发 Agent 最后一步是给 InfoBar / GamePage 加临时色块定位，尚未反馈

**调试方法（按顺序执行并汇报每一步结果）**：

**Step A — 色块定位**：给以下 widget 临时设不同背景色，截图或描述哪个颜色的区域宽度不对。
```cpp
// InfoBar 构造末尾（仅调试，最终提交前删除）
setStyleSheet("background: #ff0000;");  // 红色：InfoBar 自身

// GamePage 构造中，root layout 创建后：
// 给 GamePage 自身加色
setStyleSheet("background: #0000ff;");  // 蓝色：GamePage 自身

// 给 m_view 加色（在 body layout 添加之后）
m_view->setStyleSheet("background: #00ff00;");  // 绿色：GameView 区域
```
预期：InfoBar 红色条应横跨整个窗口宽度。如果不是 → 根因在 GamePage 或上游（QStackedWidget）。如果是 → 根因在 InfoBar 内部 layout。

**Step B — 如果根因在 InfoBar 内部**：检查各 QLabel 的 sizePolicy。项目全局 QSS `QLabel { background: transparent; }` 不会影响布局。但 QLabel 默认 sizePolicy 是 `Preferred`，宽度由文本决定——Score 列的值 "0" 只有几个像素宽。**但这不影响 QHBoxLayout 对列的分配，因为列是 QVBoxLayout (not a widget)，QHBoxLayout 会将空间分配给 layout items。**

实际上，QHBoxLayout 中直接 `addLayout(col)` 时，QVBoxLayout 会得到空间分配。但 `QVBoxLayout` 内部的 QLabel 宽度小 → QVBoxLayout 的 sizeHint 宽度小 → 在 QHBoxLayout 中每个列只占很小的宽度。**加上末尾 Stretch 吃掉了全部剩余空间，导致列挤在左侧。**

→ 这就是根因：**不是"缩在左上角"，而是"列宽度由内容文本决定 + Stretch 吃掉剩余空间 = 列组看起来挤在左侧"**。

**Step C — 修复**：执行 §1 的对齐方式调整（首尾各加 Stretch，spacing=20）。如果列组居中后仍显拥挤，考虑给 value QLabel 设 `setMinimumWidth(48)` 或 `setAlignment(Qt::AlignCenter)` 让每列宽度一致。

### 5. 调试色块清理

提交前必须删除所有临时 `setStyleSheet` 调用。`src/app/` 中零 `setStyleSheet`。

---

## 验收标准

1. 编译 /W4 零警告
2. 45 测试全绿
3. InfoBar 六列**居中聚拢**，两侧等距留白，横向撑满
4. InfoBar 底部可见 1px 绿色暗线边框
5. LeaderboardWidget 左侧可见 1px 绿色暗线边框
6. QSS 集中在 main.qss，`src/app/` 零 `setStyleSheet`
7. 目测与 `docs/prototypes/ui-mockup-v4.html` Playing 模式的 HUD + Sidebar 视觉一致
8. `setMode(false)` 后 Kills/Rank 列正确隐藏，剩余四列重新居中

---

## 提交格式

```
改动文件清单:
~ src/app/InfoBar.cpp
~ src/resources/style/main.qss

（如果调试发现根因在 GamePage 或 LeaderboardWidget，追加对应文件）

一句冒烟:
编译 /W4 零警告，45 测试全绿，SnakeArena.exe InfoBar 居中撑满 + 底部边框 + Leaderboard 左侧边框可见。
```
