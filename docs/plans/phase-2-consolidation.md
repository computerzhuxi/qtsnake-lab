# Phase-2 巩固重构 开发设计文档

> 版本：v0.2（已批准）  
> 作者：主程  
> 状态：锁定，进入实施阶段  
> 关联：`docs/design.md`（最终设计单源）、`docs/architecture.md`（实际架构）

---

## 1. 目标（What）

在 **不引入新模块**（不做 AI / network / replay / i18n）的前提下，把已落地的 `core / controller / app / tests` 推到 **面试作品集** 应有的质量水位。

| 维度 | 目标 |
|---|---|
| 正确性 | 消除所有已发现的 UB / 内存安全隐患 |
| 可测性 | core + controller 测试行覆盖 ≥ 80%，引入 Controller 状态机测试 |
| 可演进性 | 为 AI / network / replay 注入预留确定性接口（种子、State 快照、输入抽象） |
| 可读性 | 头文件注释统一、模块边界单一职责、QSS 集中 |
| 工程门面 | 顶层 README、SnakeServer 占位 target、编译警告升级 |

## 2. 范围（In / Out）

| In Scope | Out of Scope |
|---|---|
| `core/` 数据结构与不变量 | 新增 AI / network / replay 模块 |
| `controller/` 状态机与组件 | UI 视觉大改 |
| `app/` 生命周期与事件分发 | 多语言 i18n（移到 Phase-6） |
| `tests/` 覆盖率与集成测试 | 跨平台音频接入 |
| 文档：README、`plan.md` 归档、`docs/AGENTS.md` | 视觉重构、动画 |

## 3. 现存问题清单

> **严重程度**：🔴 必须修 / 🟡 建议修 / 🟢 锦上添花

### 3.1 内存与生命周期（🔴）

- **AS-1** `AppShell::~AppShell()` 手动 `delete m_controller`，但 `m_controller` 以 `this` 为 `QObject` 父对象——**双重释放隐患**。
- **AS-2** `AppShell::showMenu()` / `startSinglePlayer()` 同样手动 `delete m_controller`。
- **AS-3** `AppShell::m_inputComponent` 为裸指针副本；`delete m_controller` 后悬空，但 `keyPressEvent` 可能在事件队列里访问——**UAF 风险**。
- **CT-1** `GameController::handleReadyKey` 用局部 `new QTimer + lambda(this)`，未存为成员；倒计时中"返回菜单 → 析构 Controller"会触发回调访问已释放对象——**崩溃风险**。

### 3.2 数据流与确定性（🔴）

- **CT-2** `GameController::startGame` 自带 `std::random_device` 决定 spawn 点，**无种子注入接口**，未来回放无法重现 spawn。
- **CT-3** Render 在 update() 首位，**死亡帧不再渲染**——这是 **有意为之** 的 UX（玩家停留在撞墙前的最后合法位置），但文档表述含糊，需澄清。
- **SN-1** `Snake::setDirection` 与 `InputComponent::setDirection` **双重 180° 反转防护**——职责分散，需统一到 Snake 层。

### 3.3 数据结构（🟡）

- **PT-1** `Point` 哈希极弱：`hash<int>(x) ^ (hash<int>(y) << 1)`，对 `(a,b)` 与 `(b,a)` 等存在显著碰撞，影响 `freeCells` 性能。换 `hash_combine` 风格。
- **SN-2** `Snake` 构造反向延伸 2 格无边界保护——调用者契约缺失，需头注释明确。

### 3.4 视图 & 体验（🟡）

- **AP-1** 窗口 resize 后未重新 `fitInView`，棋盘比例失真。
- **AP-2** `GamePage::showCountdown` 内联 `setStyleSheet`，违反 "QSS 集中" 原则。
- **AP-3** `PauseWidget → GamePage → AppShell` 三层信号原样转发，可裁剪（暂不在本 Phase 处理，记为遗留）。

### 3.5 测试覆盖（🟡）

- **TS-1** 无 `GameController` 状态机测试。
- **TS-2** 无完整一局生命周期集成测试。
- **TS-3** 无 `Point` 哈希分布测试 / `FoodComponent` 种子确定性测试。

### 3.6 工程门面（🟢）

- **DOC-1** `plan.md` 与 `design.md` 内容 70% 重复，需归档 `plan.md`。
- **DOC-2** 仓库无 `README.md`。
- **CM-1** 顶层 `CMakeLists.txt` 未定义 `SnakeServer.exe` 占位 target。
- **CM-2** 缺严格编译警告（`/W4` / `-Wall -Wextra`）。

## 4. 目标架构差异（Before / After）

| 维度 | Before | After |
|---|---|---|
| 控制器生命周期 | 裸指针 + 手动 delete + Qt parent | `std::unique_ptr<GameController>`；构造不传 parent |
| 输入指针 | `AppShell` 缓存 `m_inputComponent` | 删除缓存；使用处即时 `m_controller->input()` |
| 倒计时 | 局部 `new QTimer` + lambda | 成员 `QTimer* m_countdownTimer`，由 `reset()` / 析构统一停止 |
| 随机性入口 | `startGame` 自带 `random_device` | 引入 `RngService`（mt19937，可注入种子），spawn 与 food 均依赖它 |
| 死亡帧渲染 | 文档表述模糊 | 文档显式声明 "停在撞墙前合法位置" 为 UX 策略 |
| QSS | 部分内联 | 完全集中于 `main.qss`；倒计时改用 `objectName` |
| 测试 | 局部组件单元测试 | 增加 Controller 状态机测试 + 完整一局集成测试 |
| 文档 | plan + design 重复 | 保留 `design.md`（设计单源） + `architecture.md`（实际架构）；`plan.md` 归档 |
| 顶层 CMake | 仅 `SnakeArena` | 增加 `SnakeServer` 占位 target；启用严格警告 |

## 5. 风险与权衡

| 风险 | 缓解 |
|---|---|
| 引入 `RngService` 牵动 controller / food / 未来 ai 多处 | 这是 Phase-3 AI / Phase-5 回放的前置依赖，越晚做改动越大 |
| 改 `Point` 哈希影响所有 `freeCells` 依赖 | 不影响功能；重跑全部 `freeCells` 相关测试 |
| 移除 `InputComponent::setDirection` 反转防护 | 必须保留 `Snake` 层那道，并加测试 |
| 严格警告升级可能暴露既有警告 | 一并修复，作为 T-10 的一部分 |

## 6. 任务清单（10 张 Task Card）

| ID | 标题 | 改动幅度 |
|---|---|---|
| T-01 | 修复 AppShell / GameController 生命周期 | 中 |
| T-02 | 修复倒计时定时器悬空风险 | 小 |
| T-03 | 引入 `RngService`，集中随机性 | 中 |
| T-04 | 修复 `Point` 哈希 | 小 |
| T-05 | 统一蛇方向防护，移除双重逻辑 | 极小 |
| T-06 | 文档澄清"死亡帧渲染策略" | 小 |
| T-07 | QSS 集中化（清除内联样式） | 小 |
| T-08 | 视图自适应（resize fitInView） | 极小 |
| T-09 | 新增测试套件 `test_game_controller.cpp` | 中 |
| T-10 | 工程门面：README + SnakeServer 占位 + 编译警告 | 中 |

执行顺序：**严格串行**（用户指定 1 个开发 Agent）。

预计周期：作品集质量水位下，2–3 个工作日。

详细任务卡见 `docs/tasks/T-XX-*.md`。

## 7. 后续 Phase 路线

| 阶段 | 依赖 | 备注 |
|---|---|---|
| Phase-3 AI 对战 | T-03 `RngService` | BFS + 跟随尾巴 |
| Phase-4 联机对战 | T-09 状态机测试基线 | TCP + 二进制协议 |
| Phase-5 回放 | T-03 / T-09 | 操作 + 种子录制 |
| Phase-6 i18n + 音效落地 | 无强依赖 | QTranslator + QSoundEffect |
| Phase-7 发布门面 | 与 T-10 衔接 | 截图 / GIF / CI |
