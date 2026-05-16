# Snake Arena 项目总体规划 (plan.md)

## 1. 项目最终形态

基于 Qt5 的多模式贪吃蛇对战平台，四种模式：

- **单人经典模式**：键盘操控，吃食物增长，撞墙或撞自己结束。
- **AI 对战模式**：可选择 AI 难度与数量，AI 使用 BFS 寻路 + 跟随尾巴策略。
- **联机对战模式**：支持两种连接方式——
  - **局域网直连**：一名玩家创建房间（内嵌 GameServer），显示本机 IP，其余玩家通过局域网 IP 加入。
  - **公共服务器**：SnakeServer 独立部署在拥有公网 IP 的机器上，客户端通过 IP 连接。
- **对局回放模式**：可录制并回放任意模式的对局。回放文件自动标识对局类型，播放时无需手动选择模式。支持播放/暂停/倍速。

构建产物两个可执行文件：
- **SnakeArena.exe**（客户端）——玩家使用的完整程序。
- **SnakeServer.exe**（服务端）——纯网络服务进程，部署到服务器，无 GUI，与客户端共用 network/core 模块。

视觉风格采用**暗色电竞 / TRON 风格**：蛇身为方块造型带霓虹发光，食物为圆形带光晕，整体暗色背景 + 发光配色。

---

## 2. 模块划分与职责

```
src/
├── core/        # 纯 C++ 游戏模型，不依赖 Qt
│                #   蛇、棋盘、碰撞、食物、随机数种子管理
├── ui/          # QGraphicsView 游戏渲染
│                #   SnakeItem, FoodItem, GameScene, GameView
├── app/         # 应用外壳 UI（自定义暗色风格浮层，替代原生控件）
│                #   主菜单、暂停/选项弹窗、结算弹窗、设置、AI配置、
│                #   联机弹窗、回放列表、房间大厅、多语言切换入口
├── controller/  # 游戏控制器（驱动循环、模式管理、输入整合）
├── ai/          # AI 决策（BFS 寻路 + 跟随尾巴）
├── network/     # 联机通信（QTcpServer/QTcpSocket、二进制协议、内嵌/独立 GameServer）
├── replay/      # 录制与回放（操作+种子录制、回放驱动、对局类型标识）
├── logging/     # 日志系统（级别控制、控制台/文件输出、模块标签）
├── audio/       # 音效接口（先做接口层，底层待对接 QSoundEffect）
└── resources/   # 打包资源（QSS 样式、字体、翻译 .qm、音频文件）
```

各模块编译为静态库，顶层 CMake 链接成两个 target。依赖方向：

```
app ─── ui ─── controller ─── core
  │       │
  ├── ai ─┘
  ├── network ─── core ─── logging
  ├── replay ──── core
  ├── audio
  └── resources

SnakeServer.exe: network → core → logging  （仅这三个模块）
```

---

## 3. 核心流程

```
[输入] 键盘 / AI决策 / 网络包 / 回放指令
       ↓
[GameController::tick()]  每100~150ms触发
       │
       ├─ 处理输入，更新所有蛇的方向
       ├─ 执行移动、碰撞检测、食物生成
       ├─ 更新 GameScene (syncFromBoard)
       └─ 若联机服务器，打包 WorldState 广播
```

所有模式复用同一套 GameController 与 Board，仅输入来源不同：
- 单人：键盘 → 控制器
- AI：AI 控制器 → 控制器
- 联机客户端：网络包 → 控制器
- 回放：文件读取 → 控制器

---

## 4. 应用外壳架构

游戏画面始终在窗口底层运行（QGraphicsView），所有界面作为浮层覆盖在上方，通过 show()/hide() 控制显隐：

```
QWidget (顶层窗口)
  ├── QGraphicsView    ← 游戏画面，始终最底层
  ├── MainMenuWidget   ← 主菜单（全屏浮层）
  ├── PauseWidget      ← 暂停/选项浮层
  ├── GameOverWidget   ← 结算浮层
  ├── SettingsWidget   ← 设置（全屏浮层）
  └── ... 其他弹窗
```

浮层实现为自定义 QWidget，半透明遮罩背景，弹窗居中显示。

### 4.1 UI 渲染方式

- **游戏画面**：QGraphicsView + QGraphicsScene + QGraphicsItem
- **所有界面 UI**：QSS（Qt Style Sheets），全局样式文件 `src/resources/style/main.qss`，各组件的差异化样式通过 `objectName` 精准覆盖

### 4.2 倒计时

- **单机模式**：按任意键准备 → 屏幕中央 3 → 2 → 1 → GO → 游戏开始
- **联机模式**：全部玩家准备 → 房主点开始 → 3 → 2 → 1 → GO → 游戏开始

---

## 5. 界面流转

### 5.1 单机（单人/AI）

```
主菜单
  ├─ 单人经典 → 主菜单消失 → 按任意键 → 3-2-1 → 游戏开始
  └─ AI 对战  → AI 配置弹窗 → 主菜单消失 → 按任意键 → 3-2-1 → 游戏开始

游戏中：
  ESC → 暂停弹窗（继续/重新开始/设置/返回主菜单）
  蛇死亡 → 结算弹窗（分数+统计/再来一局/保存回放/返回主菜单）
```

### 5.2 联机（局域网/服务器）

```
主菜单
  ├─ 局域网对战
  │     ├─ 创建房间 → 内嵌服务器启动 → 显示本机IP → 房间大厅
  │     └─ 加入房间 → 输入IP+端口 → 房间大厅
  │
  └─ 服务器对战
        └─ 输入服务器IP → 连接 → 房间列表 → 选择/创建 → 房间大厅

房间大厅：
  ├─ 玩家列表 + 各玩家准备状态
  ├─ 房主可设置：棋盘大小、游戏速度、网格开关、食物闪烁
  ├─ 全部准备 → 房主开始 → 3-2-1 → 游戏开始

游戏中：
  ESC → 选项弹窗（不暂停游戏：继续/游戏设置只读/离开房间）
  蛇死亡 → 结算弹窗（排名+统计/保存回放/返回主菜单）
```

### 5.3 对局回放

```
主菜单 → 回放列表弹窗 → 选择文件 → 播放（支持播放/暂停/倍速）
```

---

## 6. 关键技术选型与理由

| 技术点 | 选择 | 一句话理由 |
|--------|------|-----------|
| GUI 框架 | Qt5 Widgets | 成熟稳定，QGraphicsView 适合游戏循环 |
| 图形架构 | QGraphicsView / Scene / Item | 内置碰撞检测、坐标变换，蛇和食物管理清晰 |
| UI 样式 | QSS（全局样式表） | 统一暗色电竞风，不依赖系统原生样式，修改方便 |
| 应用外壳 | 游戏底层 + 浮层叠加 | 接近游戏体验，菜单/暂停切换自然 |
| 游戏循环 | QTimer | 与 Qt 事件循环天然融合，无需手动管理线程 |
| AI 算法 | BFS + 跟随尾巴 | 经典、可靠，展现搜索算法能力且不依赖外部库 |
| 网络模型 | 权威服务器 (C/S over TCP) | 防作弊、同步简单，TCP 保证指令和状态可靠 |
| 网络 API | QTcpServer / QTcpSocket | 信号槽异步编程，无需手动处理 select/epoll |
| 序列化 | QDataStream + 自定义二进制消息 | 轻量高效，跨平台字节序可控，易于测试 |
| 随机数 | std::mt19937（可传入种子） | 保证回放确定性，比 rand() 质量高 |
| 测试 | Google Test | 独立测试核心逻辑，集成 CMake 方便 |
| 多语言 | QTranslator + Qt Linguist | 运行时切换中英文，无需重启 |
| 资源打包 | Qt .qrc | 样式/字体/翻译/音频全打包进 exe，不依赖外部文件 |
| 服务端 | 独立可执行文件 | 与客户端共用 network/core 代码，无 GUI |
| 音效 | AudioManager 接口先行 | 预留接口，底层后续对接 QSoundEffect |

---

## 7. 模块依赖关系图

```
          ┌──────────────┐
          │  main.cpp    │
          └──────┬───────┘
                 │
          ┌──────▼───────┐
          │   app/       │ (浮层管理、菜单、弹窗、设置)
          │  (UI Shell)  │
          └──────┬───────┘
                 │
  ┌──────────────┼──────────────────┐
  │              │                  │
┌─▼──────────┐ ┌─▼──────────┐ ┌────▼──────────┐
│ GameView   │ │ GameServer │ │ ReplayPlayer  │
│ (单人/AI)  │ │ (联机模式) │ │  (回放模式)   │
└─────┬──────┘ └─────┬──────┘ └─────┬──────────┘
      │              │              │
      └──────────────┼──────────────┘
                     │
              ┌──────▼──────┐
              │GameController│ (核心协调器)
              └──┬───┬───┬──┘
                 │   │   │
       ┌─────────┘   │   └─────────┐
       ▼             ▼             ▼
 ┌──────────┐  ┌──────────┐  ┌──────────┐
 │  core/   │  │  ai/     │  │ network/ │
 │ (模型)   │  │ (决策)   │  │ (通信)   │
 └──────────┘  └──────────┘  └──────────┘
       │
       ▼
 ┌──────────┐
 │   ui/    │ (渲染)
 └──────────┘
```

注：audio/ 和 logging/ 被各模块按需引用，resources/ 在顶层打包，图中从略。

---

## 8. 设置页面内容

- **操作标签**：键位自定义（默认方向键，可改 WASD）、语言切换（中文/英文，运行时生效）
- **游戏标签**：游戏速度（慢/中/快）、棋盘大小（小/中/大）、音量、显示网格（开关）、食物闪烁效果（开关）

---

## 9. 文件与目录结构

```
SnakeArena/
├── CMakeLists.txt
├── src/
│   ├── main.cpp                     # 客户端入口
│   ├── main_server.cpp              # 服务端入口
│   ├── core/
│   │   ├── CMakeLists.txt
│   │   ├── Point.h
│   │   ├── Snake.h / Snake.cpp
│   │   ├── Board.h / Board.cpp
│   │   └── Direction.h
│   ├── ui/
│   │   ├── CMakeLists.txt
│   │   ├── GameScene.h / .cpp
│   │   ├── GameView.h / .cpp
│   │   ├── SnakeItem.h / .cpp
│   │   └── FoodItem.h / .cpp
│   ├── app/
│   │   ├── CMakeLists.txt
│   │   ├── MainMenuWidget.h / .cpp
│   │   ├── PauseWidget.h / .cpp
│   │   ├── GameOverWidget.h / .cpp
│   │   ├── SettingsWidget.h / .cpp
│   │   ├── AIConfigWidget.h / .cpp
│   │   ├── MultiplayerWidget.h / .cpp
│   │   ├── ServerBrowserWidget.h / .cpp
│   │   ├── RoomLobbyWidget.h / .cpp
│   │   ├── ReplayBrowserWidget.h / .cpp
│   │   └── AppShell.h / .cpp         # 浮层管理与页面切换
│   ├── controller/
│   │   ├── CMakeLists.txt
│   │   └── GameController.h / .cpp
│   ├── ai/
│   │   ├── CMakeLists.txt
│   │   └── BFSAIController.h / .cpp
│   ├── network/
│   │   ├── CMakeLists.txt
│   │   ├── Protocol.h / .cpp
│   │   ├── GameServer.h / .cpp
│   │   └── GameClient.h / .cpp
│   ├── replay/
│   │   ├── CMakeLists.txt
│   │   ├── ReplayRecorder.h / .cpp
│   │   └── ReplayPlayer.h / .cpp
│   ├── logging/
│   │   ├── CMakeLists.txt
│   │   └── Logger.h / .cpp
│   ├── audio/
│   │   ├── CMakeLists.txt
│   │   └── AudioManager.h / .cpp
│   └── resources/
│       ├── resources.qrc
│       ├── style/main.qss
│       ├── fonts/
│       ├── audio/
│       └── i18n/
│           ├── snake_zh.ts
│           └── snake_en.ts
├── tests/
│   ├── CMakeLists.txt
│   ├── test_snake.cpp
│   ├── test_board.cpp
│   ├── test_ai.cpp
│   └── test_protocol.cpp
├── docs/
│   ├── plan.md            ← 本文件
│   ├── architecture.md    (将来)
│   └── notes/             (里程碑微文档)
└── README.md
```

---

## 10. 多语言方案

- 代码中所有用户可见字符串使用 `tr()` 包裹
- 翻译源文件 `.ts` 使用 Qt Linguist 管理，编译产物 `.qm` 打入 `.qrc`
- 设置界面选择语言 → `QTranslator::load()` 热切换 → `LanguageChange` 事件刷新全部 UI
- 无需重启程序

---

## 11. 配色参考

| 用途 | 颜色 | 说明 |
|------|------|------|
| 背景（最深） | `#06060f` | 窗口/棋盘底色 |
| 面板背景 | `#0a0a1a` / `#08081a` | 弹窗/信息区 |
| 按钮常态 | `#0d0d28` | 按钮背景 |
| 按钮边框 | `#222255` | 默认边框 |
| 高亮边框 | `#00ff88` | 悬停/选中 |
| 文字主色 | `#ccc` | 普通文字 |
| 文字亮色 | `#e0e0ff` | 标题文字 |
| 文字暗色 | `#556` | 辅助信息 |
| 蛇身 | `#00ff88` → `#00bb55` | 头部到尾部渐变 |
| 食物 | `#ff4466` / `#ffaa00` | 不同类型 |
| 危险/退出 | `#ff6688` | 退出按钮、游戏结束标题 |
