# Snake Arena

> 基于 Qt5 的多模式贪吃蛇对战平台 | A Qt5-based multi-mode snake battle platform

## Screenshots | 截图

<!-- TODO: add screenshots to docs/screenshots/ -->

| Menu | In-Game | Pause | Game Over |
|------|---------|-------|-----------|
| ? | ? | ? | ? |

## Modules | 模块

| Module | Responsibility | 职责 |
|--------|---------------|------|
| **core/** | Pure data (Point, Board, GameState) + Snake | 纯数据 + Snake 行为 |
| **logging/** | Async logger (thread + queue) | 异步日志 |
| **ui/** | QGraphicsItem rendering, GameScene | 渲染 |
| **controller/** | Game loop driver + 5 components (Input/Move/Collision/Food/Render) | 控制器 + 组件 |
| **app/** | QStackedWidget pages + overlays + keyboard dispatch | 应用外壳 + 浮层 |
| **audio/** | Audio interface (stub) | 音效接口 |
| **resources/** | QSS stylesheet + .qrc | 样式资源 |

## Build | 构建

### Windows (MSVC)

```bash
cmake -B build -G "Visual Studio 17 2022"
cmake --build build --config Release
```

### Linux (GCC)

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### Targets

| Target | Description |
|--------|-------------|
| `SnakeArena.exe` | Full client (GUI + game + UI) |
| `SnakeServer.exe` | Headless server (stub, Phase-3) |

### Tests | 测试

```bash
cmake --build build --config Debug --target test_core
ctest -C Debug
```

## Roadmap | 路线图

| Phase | Scope | Status |
|-------|-------|--------|
| Phase-0 | Skeleton + CI pipeline | Done |
| Phase-1 | Single-player core (snake, food, collision, render) | Done |
| Phase-2 | Consolidation: QSS, lifecycle, RNG, hash, docs | In Progress |
| Phase-3 | AI (BFS pathfinding) + Replay | Planned |
| Phase-4 | Multiplayer (TCP server/client) | Planned |
| Phase-5 | Deterministic replay + polish | Planned |

## License | 许可证

MIT / TBD
