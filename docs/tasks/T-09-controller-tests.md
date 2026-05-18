# T-09 — 新增测试套件 `test_game_controller.cpp`

> Phase-2 · 改动幅度：中 · 串行序号：9 · 前置：T-01、T-02、T-03

## 来自 T-02 审查的强化项（主程备忘）

T-02 已经新建了 `tests/test_game_controller.cpp` 的骨架，包含 2 个用例。T-09 在此基础上**必须**追加以下强化：

- **TEST-1（QApplication 生命周期）**：当前 `tests/test_game_controller.cpp:8-11` 使用全局静态 `QApplication g_app(...)`，依赖链接器把它在 `gtest_main::main()` 之前调起，在 MSVC + GTest 配置下能跑但工具链脆弱。T-09 必须把 QApplication 迁移到下列之一：
  - `::testing::Environment` 子类，在 `SetUp()` 中构造、`TearDown()` 中析构；或
  - 自定义 `main()` 替换 `gtest_main`，在其中构造 QApplication。
- **TEST-2（防叠加测试不够强）**：当前 `repeatedHandleReadyKeyDoesNotStack` 实际依赖 `handleReadyKey()` 的 Ready-state 早退路径，未真正验证 `disconnect()` 起效。T-09 必须新增：
  - 一个"强制重连"测试：例如通过 `controller.reset()` 把状态回 Idle 后再 `startGame()` + `handleReadyKey()` 多次，验证 `m_countdownTimer` 的 timeout 连接数最终仍为 1（可用 `QSignalSpy` 或对 `countdownTick` 信号在固定时间窗内的总发射次数做更紧的上界断言）。


## 任务描述

补齐 `GameController` 状态机的回归测试，并为后续 Phase（AI / network / replay）建立 "Controller 行为契约" 的测试基线。

## 输入约束

- 新建文件，仅限：
  - `tests/test_game_controller.cpp`
- 可修改文件，仅限：
  - `tests/CMakeLists.txt`（追加新测试源，**必要时** 链接 `Qt5::Core` 用于 `QCoreApplication`）
- **不得修改 `src/` 任何文件。**
- 不得新增 mock / 不得让生产代码暴露内部状态——所有断言走公共 API。

## 输出约束

### 测试入口

- 测试 main 函数中**显式构造** `QCoreApplication app(argc, argv);`，便于 `QTimer` / 事件循环工作。
- 推进事件循环的辅助：
  - 自定义本地工具函数 `void pumpFor(int ms)`，内部用 `QEventLoop` + `QTimer::singleShot(ms, &loop, &QEventLoop::quit); loop.exec();`。
  - **禁止** 使用 `std::this_thread::sleep_for`。
  - **禁止** 使用 `QTest::qWait`（无 QtTest 依赖）。

### 必含用例（最少 12 个）

按主题分组：

1. **状态机迁移**
   - `idleToReady`：构造后状态为 `Idle`；`startGame()` 后变 `Ready`。
   - `readyToCountdown`：`Ready` 下 `handleReadyKey()` 后变 `Countdown`。
   - `countdownToPlaying`：`Countdown` 下 `pumpFor(2500)`（≥3×700ms）后变 `Playing`。
   - `playingToPausedAndBack`：`Playing` 下 `pause()` 变 `Paused`；`resume()` 回 `Playing`。
   - `pauseNoOpInNonPlaying`：`Idle` / `Ready` / `Paused` / `GameOver` 下调用 `pause()`，状态不变。
   - `resumeNoOpInNonPaused`：类似规约。

2. **重置与悬空保护**
   - `resetCancelsCountdown`：`handleReadyKey()` 立即 `reset()`，`pumpFor(3000)`，期间 `countdownTick` 信号 **总发射次数 ≤ 1**（即只允许首次同步发射）。
   - `multipleHandleReadyKeyNoStack`：连续 `handleReadyKey()` × 3，`pumpFor(3000)`，`countdownTick(0)` 信号被发射次数 **≤ 1**（不叠加多组 3-2-1）。

3. **游戏结束**
   - `collisionTriggersGameOver`：用足够小棋盘 + 朝墙方向初始化，`pumpFor` 推进直到 `state() == GameOver`，断言 `gameState().gameOver == true`、`scoreChanged` 信号至少发射 1 次。

4. **确定性**（依赖 T-03）
   - `sameSeedSameFoodSequence`：两个 `GameController` 实例 `setSeed(42)` → `startGame(10,10,...)`，比较 `gameState().board.foodPos` 完全相同；再 `handleReadyKey()` + `pumpFor(3000)` 后再次比较，至少前 3 次食物坐标一致。
   - `setSeedIgnoredOutsideIdle`：`startGame()` 后再 `setSeed(123)`，验证食物序列**不**被改变（断言对比"startGame 后立即 spawn 的 foodPos"）。

5. **信号契约**
   - `stateChangedSignalEmits`：用 `QSignalSpy` 监听 `stateChanged`，从 `Idle → Ready → Countdown → Playing → GameOver` 全程的发射次序与值正确。
   - `scoreChangedOnStartAndDeath`：`startGame` 后立即收到 `scoreChanged(0)`；`GameOver` 时再次收到 `scoreChanged`。

> **共 12 项**；用例命名建议使用上述键名 `TEST(GameController, idleToReady)` 等。

## 验收标准

- 整体测试运行时间 < 10 秒（含上述用例）。
- 全部用例通过；无 flaky（连跑 3 次全绿）。
- 行覆盖率：`GameController.cpp` ≥ 80%（人工估算/工具任选，PR 描述声明即可）。
- 不修改 `src/`，证据：`git diff --stat src/` 在 PR 中显示无变化。

## 不允许做的事

- 不允许暴露 `GameController` 私有成员（不加 `friend`、不加 getter 仅服务于测试）。
- 不允许引入 `QtTest` / `gmock`。
- 不允许 sleep。
- 不允许把倒计时间隔 700ms 改成 0 以加速测试（保持生产行为）；用 `pumpFor(2500)` 等待。

## 反问提示

- 若 `tests/CMakeLists.txt` 当前未链接 `Qt5::Core`，**反问主程**是否允许追加（默认答案：**允许**，仅链接 `Qt5::Core`，不引入 Widgets）。
- 若发现 `RenderComponent` 在测试环境下因 `GameScene == nullptr` 而崩溃，**反问主程**：是否允许在测试中传入 `nullptr` 并让 `RenderComponent` 容忍空 sink（默认答案：**不允许改生产代码**，测试侧需构造可丢弃的 `GameScene`，或反问要不要把 `RenderComponent` 容忍 nullptr 作为单独小卡）。
