# Snake Arena 项目进度概览

> 主程（Tech Lead）维护。  
> 每张 Task Card 状态变更由主程在审查通过 / 打回时同步更新。  
> 时间戳精确到日；详细审查意见见对应 PR / 对话记录。

---

## Phase 路线总览

| 阶段 | 状态 | 起 / 止 | 备注 |
|---|---|---|---|
| Phase-1 基础骨架（core / ui / controller / app 主流程） | ✅ 已完成 | — / 2026-05 前 | 现有代码与测试 |
| **Phase-2 巩固重构** | ✅ 已完成 | **2026-05-18 ~ 2026-05-19** | 10 正式 + 3 hotfix = 13 张，46 测试全绿 |
| **Phase-3 打磨（UI + 碰撞重构）** | **🔄 进行中** | **2026-05-19** | 分支 `phase-3-collision-refactor`；T-11~T-13 铺设接口，T-13a 增量位掩码重构，T-14~T-15 收尾 |
| Phase-4 联机对战 | ⏸ 未启动 | — | 依赖 T-09 状态机测试基线 |
| Phase-5 回放 | ⏸ 未启动 | — | 依赖 T-03 / T-09 |
| Phase-6 i18n + 音效落地 | ⏸ 未启动 | — | 无强依赖 |
| Phase-7 发布门面（截图 / GIF / CI） | ⏸ 未启动 | — | 与 T-10 衔接 |

---

## Phase-2 Task Card 状态板

> 状态值：`pending`（待启动） / `in_progress`（进行中） / `review`（待审查） / `passed`（已通过） / `rejected`（已打回，括注次数）

| ID | 标题 | 状态 | 领卡人 | 开始 | 结束 | 备注 |
|---|---|---|---|---|---|---|
| T-01 | 修复 AppShell / GameController 生命周期 | `passed` | dev-agent | 2026-05-18 | 2026-05-18 | 全部验收项通过；ESC 暂停 UI 视觉问题归因为 CM-3，BUG-1 归因为既存逻辑缺陷，均与 T-01 无关 |
| T-0b | 热修复：AppShell::keyPressEvent ESC 直接穿透（BUG-1） | `passed` | dev-agent | 2026-05-18 | 2026-05-18 | 快照方案 A 落地；日志验证 Playing⇄Paused 切换正常 |
| T-0a | 热修复：CMakeLists 启用 AUTORCC（QSS 资源入包） | `passed` | dev-agent | 2026-05-18 | 2026-05-18 | 方案 A 落地；用户确认日志无 QSS WARN |
| T-02 | 修复倒计时定时器悬空风险 | `passed` | dev-agent | 2026-05-18 | 2026-05-18 | 成员化 QTimer + Qt 父子托管 + stop()/disconnect/connect 链；新增 2 个测试用例 |
| T-03 | 引入 `RngService`，集中随机性 | `passed` | dev-agent | 2026-05-18 | 2026-05-18 | header-only RngService；FoodComponent + startGame 全部走 m_rng；setSeed 仅 Idle；静态扫描 `<random>` 仅命中 1 处；新增 2 测试 |
| T-04 | 修复 `Point` 哈希 | `passed` | dev-agent | 2026-05-18 | 2026-05-18 | boost::hash_combine 64-bit 风格；对称对哈希不等；400 点桶分布 max≤8 |
| T-05 | 统一蛇方向防护，移除双重逻辑 | `passed` | dev-agent | 2026-05-18 | 2026-05-18 | setDirection 退化为单行；Snake 层为唯一防护；31/31 全绿 |
| T-06 | 文档澄清"死亡帧渲染策略" | `passed` | dev-agent | 2026-05-18 | 2026-05-18 | architecture.md/design.md/RenderComponent.h 三处一致；纯文档无源码变更 |
| T-0c | 热修复：清空 src/app/ 全部 setStyleSheet | `passed` | dev-agent | 2026-05-18 | 2026-05-18 | MainMenuWidget 8 处 + GameOverWidget 1 处迁移至 main.qss；src/app/ setStyleSheet = 0 |
| T-07 | QSS 集中化（清除内联样式） | `passed` | dev-agent | 2026-05-18 | 2026-05-18 | GamePage setStyleSheet 清零；三态走 property+QSS；MainMenuWidget/GameOverWidget 遗留 |
| T-08 | 视图自适应（resize fitInView） | `passed` | dev-agent | 2026-05-18 | 2026-05-18 | resizeEvent 追加 fitInView+KeepAspectRatio；仅 4 行 |
| T-09 | 新增测试套件 `test_game_controller.cpp` | `passed` | dev-agent | 2026-05-19 | 2026-05-19 | 14 用例覆盖状态机/重置/GameOver/确定性/信号契约；lambda+QVector 替代 QSignalSpy 零外链；42/42 全绿 |
| T-10 | 工程门面：README + SnakeServer + 警告 + 归档 | `passed` | dev-agent | 2026-05-19 | 2026-05-19 | README 双语/W4 零警告/SnakeServer.exe/plan.md 归档/46 测试全绿 |

---

## Phase-3 Task Card 状态板

> 状态值：`pending`（待启动） / `in_progress`（进行中） / `review`（待审查） / `passed`（已通过） / `rejected`（已打回，括注次数）

| ID | 标题 | 状态 | 领卡人 | 开始 | 结束 | 备注 |
|---|---|---|---|---|---|---|
| T-11 | 碰撞数据结构 + 接口定义 | `passed` | dev-agent | 2026-05-19 | 2026-05-19 | 3 文件（2 新增头 + Board 改），46 测试全绿，countdownToPlaying flaky 非本卡引入 |
| T-12 | NaiveCollisionDetector + 测试 | `passed` | dev-agent | 2026-05-19 | 2026-05-19 | 6 文件变更，8 用例，54/54 全绿，算法与伪代码逐行一致 |
| T-13 | GridCollisionDetector + 测试 | `passed` | dev-agent | 2026-05-19 | 2026-05-19 | 4 文件变更，9 用例含 cross-validation，63/63 全绿 |
| T-14 | Board struct→class + Snake growPending 读即消费 | `passed` | dev-agent | 2026-05-19 | 2026-05-19 | 15 文件变更，Board.cpp 新增，63/63 全绿 |
| T-15 | CollisionComponent 纯 detect + Controller processCollisions | `passed` | dev-agent | 2026-05-19 | 2026-05-19 | detect const 纯检测，processCollisions 先死后吃，63/63 全绿 |
| T-16 | FoodSpawner + 删旧文件 + Controller 流水线收尾 | `passed` | dev-agent | 2026-05-19 | 2026-05-19 | 6 旧文件删除，46/46 全绿 |
| T-16a | Food class + Board 删食物 + 文件整理 | `passed` | dev-agent | 2026-05-19 | 2026-05-19 | Food class 集中食物状态，Board 回归空间索引，controller 打平，45/45 全绿 |
| T-17 | SnakeItem 渐变色 + 发光 | `passed` | dev-agent | 2026-05-19 | 2026-05-19 | setColor + 渐变插值 + 4 层霓虹发光，45/45 全绿 |
| T-18 | FoodItem 光晕 + GameScene 网格线 | `pending` | — | — | — | — |
| T-19 | InfoBar + LeaderboardWidget | `pending` | — | — | — | — |
| T-20 | GameOverWidget 多人排名表 | `pending` | — | — | — | — |
| T-21 | ShineLabel 反光文字 | `pending` | — | — | — | — |
| T-22 | SettingsWidget Tab 重构 + QSS 收尾 | `pending` | — | — | — | — |

---

## 审查记录

> 主程在每次审查后向下追加一行（最新在上）。

| 日期 | Task | 结论 | 审查要点 / 打回理由 |
|---|---|---|---|
| 2026-05-19 | T-15 | **通过（passed）** | CollisionComponent detect const 纯检测；Controller processCollisions 先死后吃；63/63 全绿 |
| 2026-05-19 | T-14 | **通过（passed）** | Board class 封装 grid+freeCells；Snake growPending 读即消费；MoveComponent move→读→removeHead→removeTail→placeHead；63/63 全绿 |
| 2026-05-19 | T-13 | **通过（passed）** | Grid + epoch 算法正确；9 用例含 cross-validation 100 随机状态一致；63/63 全绿 |
| 2026-05-19 | T-12 | **通过（passed）** | NaiveCollisionDetector 算法与伪代码逐行一致；8 用例覆盖全部 7 种 CollisionType；54/54 全绿 /W4 零警告 |
| 2026-05-19 | T-11 | **通过（passed）** | CollisionReport.h / ICollisionDetector.h / Board.h 全部达标；46/46 全绿 |
| 2026-05-19 | T-09 | **通过（passed）** | 14 用例覆盖状态机全路径/重置/GameOver/确定性/信号契约；lambda+QVector 替代 QSignalSpy 零外链；CMakeLists.txt 无变更；42/42 全绿（21.6s） |
| 2026-05-18 | T-08 | **通过（passed）** | resizeEvent 末尾追加 fitInView + KeepAspectRatio；m_scene / isEmpty 双重守卫；仅 4 行；31 测试全绿 |
| 2026-05-18 | T-0c | **通过（passed）** | MainMenuWidget 8 处 + GameOverWidget 1 处迁移至 main.qss；src/app/ setStyleSheet 命中数 = 0；AGENTS.md §5.3 全面达标 |
| 2026-05-18 | T-07 | **通过（passed）** | GamePage setStyleSheet 清零；三态走 setProperty + QSS 属性选择器；31 测试全绿；MainMenuWidget/GameOverWidget 遗留进入跟踪 |
| 2026-05-18 | T-06 | **通过（passed）** | 纯文档：architecture.md 死亡帧策略段落 / design.md §4.4 / RenderComponent.h 头注释三处一致；31 测试全绿 |
| 2026-05-18 | T-05 | **通过（passed）** | InputComponent::setDirection 退化为单行 m_direction = dir；Snake 层为唯一 180° 防护点；新增 2 用例验证纯缓存 + Snake 防护仍生效；31 测试全绿 |
| 2026-05-18 | T-04 | **通过（passed）** | Point 哈希改 boost::hash_combine 64-bit 风格；对称对 (a,b)/(b,a) 哈希不等；400 点桶分布 max_bucket_size ≤ 8；新增 2 用例；29 测试全绿 |
| 2026-05-18 | T-03 | **通过（passed）** | RngService 集中随机性；FoodComponent 接口变更；GameController 成员/初始化顺序正确（m_rng 先于 m_food）；setSeed 仅 Idle 生效；静态扫描 `#include <random>` 仅命中 src/core/RngService.h；新增 2 个测试。MINOR-1 与 TC-DEFECT-1 见风险/遗留 |
| 2026-05-18 | git 补救 | 提交 `5b03cd7` | `chore(phase-2)` 合并 commit，包含 T-01/T-0a/T-0b/T-02 全部源码与 Phase-2 文档基建；T-03 起一卡一 commit |
| 2026-05-18 | T-02 | **通过（passed）** | 成员化 `m_countdownTimer`（Qt 父子托管）；`handleReadyKey` 走 stop→disconnect→connect→start 链；`reset()` 显式 stop；新增 2 用例全绿；用户手工冒烟 ESC 返主菜单 ×10 无崩溃。观察 TEST-1/TEST-2 移交 T-09 强化 |
| 2026-05-18 | T-0a | **通过（passed）** | 方案 A（`set(CMAKE_AUTORCC ON)`）落地；构建日志显示 `qrc_resources.cpp` 已编译入 exe（~491KB）；用户确认运行日志无 QSS WARN |
| 2026-05-18 | T-0b | **通过（passed）** | 快照方案 A 落地；`keyPressEvent` 内 `state()` 调用次数 = 1；用户日志验证 Playing⇄Paused 切换 5 次符合预期 |
| 2026-05-18 | T-0b | 已签发 | 紧急热修复，AppShell::keyPressEvent ESC 直通穿透；排序先于 T-0a |
| 2026-05-18 | T-0a | 已签发 | 紧急热修复，启用 CMAKE_AUTORCC 让 QSS 资源入包；排在 T-0b 之后 |
| 2026-05-18 | T-01 | **通过（passed）** | 用户冒烟 B 5/5 通过；冒烟 A ESC 暂停后 UI 不可见，经诊断为 CM-3（QSS 未加载）副作用，与 T-01 代码路径无关 |
| 2026-05-18 | T-01 | 条件通过（pending smoke） | 范围/接口/生命周期/测试全部达标；AppShell.cpp 中 new/delete 已清零；GameController 构造签名移除 parent；待用户 GUI 手工冒烟（菜单↔单人 ×5、撞墙再来一局 ×5）后正式标 passed |
| 2026-05-18 | (Phase-2 启动) | — | 设计文档 v0.2 批准，10 张 Task Card 落盘 |

---

## 风险登记表

> 主程根据审查发现 / 反问交流追加。

| 日期 | 风险 | 影响 | 缓解措施 | 状态 |
|---|---|---|---|---|
| 2026-05-18 | 严格警告升级可能暴露既有警告 | 编译输出爆炸 | T-10 一并修复，必要时抑制 googletest 警告 | 监控中 |
| 2026-05-18 | `RngService` 牵动多模块 | 编译错误连锁 | T-03 范围已锁定，FoodComponent 旧 `setSeed` 接口同步删除 | 监控中 |
| 2026-05-18 | TEST-1：QApplication 全局静态生命周期脆弱 | 在某些工具链上脆弱 | ✅ T-09 已修复：迁移至 testing::Environment |
| 2026-05-18 | TEST-2：anti-stack 测试仅靠 Ready-state 早退间接验证 | 未来放开 handleReadyKey 在 Countdown 下重启则漏检 | ✅ T-09 已修复：reset+重激活路径 + 收紧断言 |
| 2026-05-18 | TEST-3：test_board.cpp 未加入 CMakeLists 源列表，4 个 Board 用例未在 CI 跑 | Board 数据层回归无保护 | ✅ **T-10 已修复：test_board.cpp 补链，46/46 全绿** |
| 2026-05-18 | TC-DEFECT-1：T-03 Task Card "输入约束" 漏列 `tests/test_game_controller.cpp`，但"验收标准"要求新增测试 | Task Card 字面不闭合 | 我（主程）补一行授权（见下方"遗留问题"中 TC-DEFECT-1 条） | 已修复 |
| 2026-05-18 | MINOR-1：T-03 确定性测试只比对首颗食物 + head + body | 测试强度略弱 | 降级 Known Issue；T-09 sameSeedSameFoodSequence 已覆盖 Playing 后比较 |

---

## 遗留问题（不进入本 Phase）

| 编号 | 描述 | 计划处理阶段 |
|---|---|---|
| AP-3 | `PauseWidget → GamePage → AppShell` 三层信号原样转发，可裁剪 | Phase-6 或专项重构 |
| SN-2 | `Snake` 构造反向延伸 2 格无边界保护 | T-06 头注释中补充契约说明（如已包含则关闭） |
| CM-3 | CMakeLists 缺 `CMAKE_AUTORCC ON`，导致 QSS 资源未编入 exe，运行时 `Failed to load QSS stylesheet`；当前游戏使用 Qt 默认浅色样式（与设计文档暗色 TRON 风不符）。**阻塞 T-07**。 | ✅ **T-0a 已修复（2026-05-18）** |
| AP-5 | `test_board.cpp` 未加入 CMakeLists 源列表，4 个 Board 用例未在 CI 跑 | ✅ **T-10 已修复（2026-05-19）** |
| AP-4 | `MainMenuWidget.cpp` (8 处) + `GameOverWidget.cpp` (1 处) 仍使用内联 `setStyleSheet`，未纳入 T-07 输入约束。T-07 仅清除 GamePage。 | ✅ **T-0c 已修复（2026-05-18）** |
| BUG-1 | `AppShell::keyPressEvent` ESC 分支存在直通穿透：Playing→Paused 后未 `return`，紧接的 `if (state == Paused)` 立即把 ESC 解释为 resume，状态在一次按键内回到 Playing。玩家**无法暂停**游戏。Pre-existing，与 T-01 无关。 | ✅ **T-0b 已修复（2026-05-18）** |
