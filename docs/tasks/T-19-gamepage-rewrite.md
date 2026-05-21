# T-19：GamePage 布局重写（InfoBar + GameView + Leaderboard）

> 签发：主程 | 日期：2026-05-19 | 状态：待领取
> 分支：`phase-3-collision-refactor`

---

## 任务描述

重写 `GamePage` 布局：顶部 InfoBar（48px 高）+ 主体 HBox（GameView:弹性 + LeaderboardWidget:170px）。新建 InfoBar 和 LeaderboardWidget 两个组件。浮层（暂停/结算/倒计时）覆盖整个 GamePage。

---

## 输入约束

### 允许修改/新增的文件

```
+ src/app/InfoBar.h
+ src/app/InfoBar.cpp
+ src/app/LeaderboardWidget.h
+ src/app/LeaderboardWidget.cpp
~ src/app/GamePage.h
~ src/app/GamePage.cpp
~ src/app/CMakeLists.txt          (追加 InfoBar.cpp + LeaderboardWidget.cpp)
~ src/resources/style/main.qss    (追加 InfoBar + Leaderboard 样式)
```

### 不允许做的事

- 不得修改 core / controller / ui 层任何文件
- 不得在 `src/app/` 中使用 `setStyleSheet`（全部走 main.qss + objectName）
- 不得修改 `AppShell`（GamePage 接口不变）
- 不得使用 `using namespace std;`

---

## 输出约束

### 1. GamePage 布局结构

```
GamePage (QWidget)
  QVBoxLayout [root, margins=0, spacing=0]
    ├─ InfoBar (m_infoBar)          — addWidget, 不设 stretch
    └─ QHBoxLayout [body, margins=0, spacing=0]  — addLayout(body, stretch=1)
         ├─ GameView (m_view)       — addWidget(m_view, stretch=1)
         └─ LeaderboardWidget       — addWidget, setFixedWidth(170)
```

浮层（m_pauseWidget / m_gameOver / m_countdownLabel）作为 GamePage 的直接子 widget，在 resizeEvent 中用 `setGeometry(0, 0, w, h)` 覆盖全页。

构造顺序：
1. new GameScene / GameView / setScene
2. 构建 root layout + body layout（如上）
3. new InfoBar / LeaderboardWidget 加入布局
4. new PauseWidget / GameOverWidget / countdown QLabel
5. connect 信号
6. 不调 show()——QStackedWidget 管理可见性

### 2. InfoBar

```
InfoBar : QWidget
  setObjectName("infoBar")
  setFixedHeight(48)

  布局: QHBoxLayout [margins=16,0,16,0, spacing=0, AlignVCenter]
    [Score列]  Separator  [Length列]  Separator  [Time列]  Separator  [Speed列]  Separator  [Kills列]  Separator  [Rank列]  Stretch

每列: QVBoxLayout [spacing=0]
  QLabel(label)  objectName="hudLabel"
  QLabel(value)  objectName 按字段（见下表）

Separator: QWidget, objectName="hudDivider", fixedSize(1, 28)
```

| 字段 | label 文本 | value objectName | 初始值 |
|---|---|---|---|
| Score | `Score` | `scoreValue` | `"0"` |
| Length | `Length` | `lengthValue` | `"3"` |
| Time | `Time` | `timeValue` | `"00:00"` |
| Speed | `Speed` | `speedValue` | `"MID"` |
| Kills | `Kills` | `killsValue` | `"0"` |
| Rank | `Rank` | `rankValue` | `"1 / 1"` |

公开方法：
- `setScore(int)` / `setLength(int)` / `setTime(int seconds)` / `setSpeed(int speedMs)` / `setKills(int)` / `setRank(int rank, int total)`
- `setMode(bool multiplayer)` — false 时隐藏 Kills 列（label+value+前一个 Separator）和 Rank 列（label+value+前一个 Separator）

隐藏实现：存 Kills/Rank 的 QLabel* 和它们前面的 Separator QWidget*，setVisible 控制。

### 3. LeaderboardWidget

```
LeaderboardWidget : QWidget
  setObjectName("leaderboard")
  setFixedWidth(170)

  布局: QVBoxLayout [margins=8, spacing=4]
    QLabel("— RANK —")   objectName="rankTitle"   align=Center
    [player rows — 由 updatePlayers 动态插入]
    Stretch
    QLabel("Alive: 1 / 1") objectName="rankAlive" align=Center
```

公开方法：
- `void updatePlayers(const std::vector<PlayerInfo>& players)`
- `PlayerInfo` = `{ QString name; int score; bool alive; bool isSelf; }`

每行结构（makePlayerRow）：
```
rankRow (QWidget, objectName="rankRow", property self=isSelf, alive=isAlive)
  QHBoxLayout [margins=6,2,6,2, spacing=6]
    QLabel(rankNum)   objectName="rankNum"   property rankLevel="gold"/"silver"/"bronze"
    QWidget(dot)       objectName="rankDot"   fixedSize(8,8), property dotAlive
    QLabel(name)       objectName="rankName"  property alive
    QLabel(score)      objectName="rankScore" property alive
```

死亡玩家：靠 QSS `[alive="false"]` 灰色处理。不用 `<s>` HTML 标签（Qt QLabel 不支持 strikethrough）。

### 4. main.qss 新增

```css
/* InfoBar */
QWidget#infoBar { background: #0a0a1a; }
QLabel#hudLabel { color: #556; font-size: 8px; letter-spacing: 2px; }
QWidget#hudDivider {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 transparent, stop:0.5 rgba(0,255,136,40), stop:1 transparent);
}
QLabel#scoreValue  { color: #00ff88; font-size: 18px; font-weight: bold; }
QLabel#lengthValue { color: #ccc;    font-size: 18px; }
QLabel#timeValue   { color: #ccc;    font-size: 18px; }
QLabel#speedValue  { color: #ccc;    font-size: 18px; }
QLabel#killsValue  { color: #ff6688; font-size: 18px; }
QLabel#rankValue   { color: #00ff88; font-size: 18px; }

/* Leaderboard */
QWidget#leaderboard { background: #08081a; }
QLabel#rankTitle { color: #556; font-size: 10px; letter-spacing: 3px; padding-bottom: 4px; }
QLabel#rankAlive { color: #556; font-size: 8px; padding-top: 4px; }
QWidget#rankRow { background: transparent; border: 1px solid transparent; border-radius: 4px; }
QWidget#rankRow[self="true"] { border-color: #225544; background: #0a1a14; }
QLabel#rankNum { color: #778; font-size: 12px; font-weight: bold; }
QLabel#rankNum[rankLevel="gold"]   { color: #ffd700; }
QLabel#rankNum[rankLevel="silver"] { color: #c0c0c0; }
QLabel#rankNum[rankLevel="bronze"] { color: #cd7f32; }
QWidget#rankDot { border-radius: 2px; }
QWidget#rankDot[dotAlive="true"]  { background: #00ff88; }
QWidget#rankDot[dotAlive="false"] { background: #444; }
QLabel#rankName { color: #ccc; font-size: 12px; }
QLabel#rankName[alive="false"] { color: #555; }
QLabel#rankScore { color: #00ff88; font-size: 12px; }
QLabel#rankScore[alive="false"] { color: #555; }
```

### 5. CMakeLists.txt

```
src/app/CMakeLists.txt 追加两行：
  InfoBar.cpp
  LeaderboardWidget.cpp
```

---

## 验收标准

1. 编译 /W4 零警告
2. 45 测试全绿
3. GamePage 布局：InfoBar(顶部48px) + GameView(左中弹性) + Leaderboard(右侧170px)
4. InfoBar 六字段正确显示，separator 渐变分隔线可见
5. `setMode(false)` 后 Kills/Rank 列及其前 separator 均隐藏
6. LeaderboardWidget `updatePlayers` 正确显示排名列表（自己高亮、死亡灰色）
7. `src/app/` 中零 `setStyleSheet`
8. QSS 集中在 main.qss

---

## 提交格式

```
改动文件清单:
+ src/app/InfoBar.h
+ src/app/InfoBar.cpp
+ src/app/LeaderboardWidget.h
+ src/app/LeaderboardWidget.cpp
~ src/app/GamePage.h
~ src/app/GamePage.cpp
~ src/app/CMakeLists.txt
~ src/resources/style/main.qss

一句冒烟:
编译 /W4 零警告，45 测试全绿，SnakeArena.exe 布局正确。
```
