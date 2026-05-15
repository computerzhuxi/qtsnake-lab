# QtSnake Lab

基于 Qt5/C++17 的贪吃蛇游戏，项目目标是系统学习完整软件设计与开发流程。

## 功能

- 单机贪吃蛇基础玩法
- 游戏状态管理（菜单 / 游玩 / 暂停 / 结束）
- 设置系统（按键绑定、网格大小、速度）
- 分辨率和 DPI 缩放
- 多线程 AI 自动寻路（A* 算法）
- 日志系统
- 局域网联机（TCP + JSON）
- 心跳和断线处理
- 回放系统（可读文本格式 `.snake_replay`）
- 截图功能（F12）
- 本地排行榜
- 测试体系（9 套，100% 通过）
- NSIS 安装包

## 技术栈

- C++17
- Qt 5.15
- Qt Widgets
- CMake 3.16+
- Qt Test

## 项目结构

```
qtsnake-lab/
├── CMakeLists.txt              # 顶级 CMake
├── README.md
├── src/
│   ├── core/                   # 纯 C++17 静态库，零 Qt 依赖
│   │   ├── include/core/
│   │   │   ├── engine/         # GameController, Snake, Grid, Food 等
│   │   │   ├── state/          # StateMachine, GameState 状态模式
│   │   │   ├── ai/             # IPathfinder, AStarPathfinder
│   │   │   ├── replay/         # ReplayData, ReplaySerializer
│   │   │   └── logging/        # Logger, LogSink, ConsoleSink, FileSink
│   │   └── src/                # .cpp 实现
│   └── app/                    # Qt 依赖应用层
│       ├── include/app/
│       │   ├── MainWindow.h, GameWidget.h, GameControllerAdapter.h
│       │   ├── settings/       # AppSettings, KeyBindings
│       │   ├── network/        # Protocol, GameServer, GameClient, Heartbeat
│       │   ├── ai/             # AIWorker (QThread), AIController
│       │   ├── replay/         # ReplayManager
│       │   ├── screenshot/     # ScreenshotManager
│       │   ├── leaderboard/    # LeaderboardData
│       │   └── dialogs/        # SettingsDialog, MultiplayerDialog, LeaderboardDialog
│       └── src/
├── tests/
│   ├── core/                   # 纯 core 测试
│   └── app/                    # Qt 集成测试
└── resources/
```

## 构建

### 依赖

- Qt 5.15（Widgets + Network + Test）
- CMake 3.16+
- Visual Studio 2019/2022（Windows）

### Debug 构建

```bash
cmake -B build/debug \
    -DCMAKE_PREFIX_PATH=<Qt5.15安装路径>/msvc2019_64 \
    -DBUILD_TESTS=ON \
    -G "Visual Studio 17 2022"

cmake --build build/debug --config Debug
```

### 运行测试

```bash
ctest --test-dir build/debug -C Debug
```

### Release + 打包

```bash
cmake -B build/release \
    -DCMAKE_PREFIX_PATH=<Qt5.15安装路径>/msvc2019_64 \
    -DCMAKE_BUILD_TYPE=Release \
    -G "Visual Studio 17 2022"

cmake --build build/release --config Release
cd build/release && cpack   # 生成 NSIS 安装包 .exe
```

## 设计要点

### 核心架构

| 模式 | 应用 |
|------|------|
| **分层架构** | `core/` 纯 C++，`app/` Qt 依赖 |
| **Observer** | GameController → GameObserver → 回放录制 / 网络同步 |
| **State** | GameState / StateMachine（菜单、游玩、暂停、结束） |
| **Strategy** | IPathfinder → AStarPathfinder |
| **Facade** | NetworkManager, ReplayManager |
| **Singleton** | Logger, AppSettings |

### 回放系统

文本格式 `.snake_replay`，可直接用编辑器打开：

```ini
[config]
grid_width=30

[initial]
rng_seed=3817263549
direction=Right
snake=15,10 14,10 13,10
food=20,12

[inputs]
1=Down
6=Right

[result]
total_ticks=42
```

通过 RNG 种子保证确定性重放。

### 网络协议

Host Authority 模型，JSON 换行分隔：

```
→ {"type":"JoinRequest","playerName":"Alice"}
← {"type":"JoinResponse","accepted":true,"gridW":30,"gridH":20}
← {"type":"GameState","snake":[[15,10]],"food":{"x":20,"y":12},"tick":42}
→ {"type":"Input","direction":"Up","tick":43}
```

## 快捷键

| 按键 | 功能 |
|------|------|
| WASD / 方向键 | 移动蛇 |
| Enter | 开始游戏 / 重新开始 |
| P | 暂停 |
| F2 | 新游戏 |
| F5 | 录制回放 |
| F8 | AI 自动寻路 |
| F11 | 全屏 |
| F12 | 截图 |

## 测试统计

| 测试 | 类别 | 内容 |
|------|------|------|
| test_engine | core | 蛇移动、碰撞、食物生成 |
| test_state | core | 状态机流转 |
| test_replay | core | 录制/回放/序列化 |
| test_ai | core | A* 寻路 |
| test_logging | core | 日志系统 |
| test_replay_integration | app | 回放端到端 |
| test_network_minimal | app | TCP 连接 |
| test_socket_raw | app | WinSock 裸测试 |
| test_core_plain | core | 全部引擎纯 C++ 测试 |
