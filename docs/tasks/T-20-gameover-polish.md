# T-20：PauseWidget / GameOverWidget 面板化 + 多人排名表

> 签发：主程 | 日期：2026-05-21 | 状态：待领取
> 分支：`phase-3-collision-refactor`
> 前置卡：T-19a（InfoBar + LeaderboardWidget 已到位）

---

## 任务描述

将 PauseWidget 和 GameOverWidget 的视觉样式修至与 `docs/prototypes/ui-mockup-v4.html` 一致：
- 两个 widget 均添加 `.panel` 面板包裹层（overlay → panel → 内容）
- PauseWidget 标题改用专用样式（22px，非主菜单 28px）
- GameOverWidget 新增多人排名表 + 单人/多人双模式

---

## 输入约束

### 允许修改/新增的文件

```
~ src/app/PauseWidget.h
~ src/app/PauseWidget.cpp
~ src/app/GameOverWidget.h
~ src/app/GameOverWidget.cpp
~ src/resources/style/main.qss
```

### 不允许做的事

- 不得修改 core / controller / ui 层任何文件
- 不得在 `src/app/` 中使用 `setStyleSheet`
- 不得修改 GamePage / AppShell（PauseWidget 和 GameOverWidget 的公开接口变更后，GamePage 调用方可能需要适配——仅限于调用参数的适配，不得改 GamePage 布局骨架）
- 不得使用 `using namespace std;`
- 不得使用 QTableWidget（QSS 难以精确控制）

---

## 输出约束

### 1. 公共数据结构

新增 `GameResult` 结构体，放在 `GameOverWidget.h` 中：

```cpp
struct GameResult {
    QString name;
    int score = 0;
    int length = 0;
    int kills = 0;
    bool alive = true;
    bool isSelf = false;
};
```

LeaderboardWidget.h 中已有的 `PlayerInfo` 不动，两者独立。

### 2. PauseWidget 重构

**HTML 参考**：
```html
<div class="board-overlay">       <!-- 全屏半透明遮罩 -->
  <div class="panel">              <!-- 居中面板：background+border+border-radius -->
    <div class="title">游戏暂停</div>   <!-- 22px, letter-spacing:4px -->
    <div style="height:20px"></div>
    <button class="btn primary">继续游戏</button>
    <button class="btn">重新开始</button>
    <button class="btn">设置</button>
    <div style="height:8px"></div>
    <button class="btn danger">返回主菜单</button>
  </div>
</div>
```

**目标结构**：
```
PauseWidget (QWidget, objectName="overlay")        ← 全屏遮罩，已有
  QVBoxLayout [AlignCenter]                         ← 已有
    panelWidget (QWidget, objectName="panel")       ← 新增
      QVBoxLayout [margins=24,20,24,20, spacing=6]
        QLabel("游戏暂停", objectName="pauseTitle", AlignCenter)
        spacing(20)
        QPushButton("继续游戏", objectName="btnPrimary")
        QPushButton("重新开始")
        QPushButton("设置")
        spacing(8)
        QPushButton("返回主菜单", objectName="btnDanger")
        Stretch                          ← 面板底部留白，若按钮不满
```

关键点：
- **panelWidget 设 `setMinimumWidth(300)` 且 `setMaximumWidth(400)`**，让面板在 overlay 中宽度适中，由外层 AlignCenter 居中
- 面板内 layout **不设 AlignCenter**——让按钮自动 fill 到 panel 宽度（HTML 的 `width:100%` 效果）
- 按钮间距走 spacing=6（HTML `.btn { margin-bottom: 6px; }`）
- 公开接口不变——信号仍为 `resumeClicked/restartClicked/settingsClicked/menuClicked`

### 3. GameOverWidget 重构

**HTML 参考（多人模式）**：
```html
<div class="board-overlay">
  <div class="panel" style="min-width:420px;">
    <div class="go-title">游 戏 结 束</div>
    <div class="go-subtitle">— 最终排名 —</div>
    <table class="result-table">
      <thead><tr><th>#</th><th>玩家</th><th>分数</th><th>长度</th><th>击杀</th><th>状态</th></tr></thead>
      <tbody>
        <tr class="you"><td>1</td><td>YOU</td><td>156</td><td>12</td><td>2</td><td>存活</td></tr>
        ...
      </tbody>
    </table>
    <button class="btn primary">再来一局</button>
    <button class="btn" disabled>保存回放</button>
    <div style="height:8px"></div>
    <button class="btn danger">返回主菜单</button>
  </div>
</div>
```

**目标结构**：
```
GameOverWidget (QWidget, objectName="overlay")      ← 全屏遮罩，已有
  QVBoxLayout [AlignCenter]                          ← 已有
    panelWidget (QWidget, objectName="panel", minWidth=360)
      QVBoxLayout [margins=24,20,24,20, spacing=6]
        QLabel("游 戏 结 束", objectName="gameOverTitle", AlignCenter)
        [contentArea]              ← 动态区域：单人=大分数+统计行 / 多人=排名表
        spacing(12)
        QPushButton("再来一局", objectName="btnPrimary")
        QPushButton("保存回放", enabled=false)
        spacing(8)
        QPushButton("返回主菜单", objectName="btnDanger")
        Stretch
```

**公开接口变更**：

```cpp
// 单人模式——大分数 + 统计行（替代原 setScore）
void setSingleResult(int score, int length, int kills, int seconds);

// 多人模式——排名表
void setMultiResults(const std::vector<GameResult>& results);
```

删除原 `setScore(int score, int length, int kills, int seconds)`，换为 `setSingleResult`（行为等价，名字更明确）。

**单人 contentArea 构建**（`buildSingleContent`）：
```
QLabel(score)      objectName="scoreLabel"    36px green bold
QLabel("长度: X  |  击杀: Y  |  时间: MM:SS")  objectName="goStats"
```
`goStats` 样式：`color: #556; font-size: 12px;`

**多人 contentArea 构建**（`buildMultiContent`）：

先放副标题：
```
QLabel("— 最终排名 —")   objectName="goSubtitle"  AlignCenter
```

排名表用 QGridLayout（不用 QTableWidget）：
- **表头行**：`#` / `玩家` / `分数` / `长度` / `击杀` / `状态`，每列 QLabel，objectName="resultHeader"
- **数据行**：遍历 results，每行 6 个 QLabel
  - rank 列：`#1` `#2` `#3`...，前3名设 property `rankLevel="gold"/"silver"/"bronze"`
  - name 列：名字文本
  - score/length/kills 列：数字文本
  - status 列：alive ? "存活" : "死亡"，alive 时 color=#00ff88, 死亡 color=#ff6688
  - 若 isSelf：整行 QLabel 设 property `you="true"`
- 所有数据格 objectName="resultCell"
- 表头下加一条分隔线：QWidget, objectName="resultDivider", fixedHeight=1

**contentArea 切换逻辑**：
- `setSingleResult` 调用时：清空 contentArea 旧内容 → 重建单人布局
- `setMultiResults` 调用时：清空 contentArea 旧内容 → 重建多人布局

实现方式：在 panel 的 QVBoxLayout 中，title 之后预留一个 contentLayout（QVBoxLayout），切换时清空其所有子项再重建。

### 4. GamePage 适配

GamePage.cpp `showGameOver` 方法签名和调用需要适配：
- 当前调用 `m_gameOver->setScore(score, length, kills, seconds)`
- 改为 `m_gameOver->setSingleResult(score, length, kills, seconds)`

GamePage.h 的 `showGameOver` 声明不变。

### 5. main.qss 新增

```css
/* PauseWidget */
QLabel#pauseTitle {
    color: #e0e0ff;
    font-size: 22px;
    font-family: monospace;
    letter-spacing: 4px;
}

/* GameOverWidget */
QLabel#goSubtitle {
    color: #556;
    font-size: 12px;
    font-family: monospace;
    padding-bottom: 8px;
}
QLabel#goStats {
    color: #556;
    font-size: 12px;
}
QLabel#resultHeader {
    color: #445;
    font-size: 9px;
    letter-spacing: 2px;
    padding: 4px 8px;
}
QLabel#resultCell {
    color: #ccc;
    font-size: 13px;
    padding: 5px 8px;
}
QLabel#resultCell[you="true"] {
    color: #00ff88;
}
QLabel#resultCell[rankLevel="gold"]  { color: #ffd700; font-weight: bold; }
QLabel#resultCell[rankLevel="silver"] { color: #c0c0c0; font-weight: bold; }
QLabel#resultCell[rankLevel="bronze"] { color: #cd7f32; font-weight: bold; }
QWidget#resultDivider {
    background: rgba(255, 255, 255, 6);
    max-height: 1px;
}
```

> `rgba(255,255,255,6)` 在 QSS 中约等于 `#0f0f0f`。如果 Qt5 解析失败，降级为 `#0a0a0a`。

---

## 验收标准

1. 编译 /W4 零警告
2. 45 测试全绿
3. PauseWidget：半透明遮罩 + 居中 panel（圆角边框）+ 标题 22px + 按钮填充 panel 宽度
4. GameOverWidget 单人：半透明遮罩 + 居中 panel + "游 戏 结 束"标题 + 大分数 + 统计行 + 三个按钮
5. GameOverWidget 多人：同 panel 结构 + "— 最终排名 —"副标题 + 6 列排名表 + 自己行高亮 + 死亡行正确显示
6. `src/app/` 中零 `setStyleSheet`
7. 目测与 `docs/prototypes/ui-mockup-v4.html` Pause / GameOver 两个状态视觉一致
8. 按钮 hover / press 交互正常（QPushButton 全局样式已在 main.qss 中定义，无需额外工作）

---

## 提交格式

```
改动文件清单:
~ src/app/PauseWidget.h
~ src/app/PauseWidget.cpp
~ src/app/GameOverWidget.h
~ src/app/GameOverWidget.cpp
~ src/app/GamePage.cpp              (适配 setSingleResult)
~ src/resources/style/main.qss

新增/修改测试列表:
无测试变更，45 全绿。

一句冒烟:
编译 /W4 零警告，45 测试全绿，SnakeArena.exe PauseWidget 居中 panel + 22px 标题 + 全宽按钮，
GameOverWidget 单人=大分数+统计行 / 多人=排名表+自己高亮+死亡灰色，
目测与 ui-mockup-v4.html Pause/GameOver 状态一致。
```
