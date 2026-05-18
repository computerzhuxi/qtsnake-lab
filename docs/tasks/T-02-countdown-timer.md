# T-02 — 修复倒计时定时器悬空风险

> Phase-2 · 改动幅度：小 · 串行序号：2 · 前置：T-01

## 任务描述

消除 `GameController::handleReadyKey` 中局部 `new QTimer + lambda(this)` 的悬空回调隐患。把倒计时定时器**提升为成员**，并保证在 `reset()` / 析构 / 状态切换时被显式 `stop()`。

## 输入约束

- 可修改文件，仅限：
  - `src/controller/GameController.h`
  - `src/controller/GameController.cpp`
- **不得新建任何文件。**
- **不得修改其他模块。**

## 输出约束

- `GameController` 新增私有成员：
  - `QTimer* m_countdownTimer = nullptr;`（以 `this` 为父，构造时一并 `new`）。
- `handleReadyKey()` 复用该成员；启动前若正运行先 `stop()`；连接信号槽使用 `Qt::UniqueConnection` 或显式断开再连。
- 该 `lambda` **只允许捕获 `this`**，不得捕获除主控制器自身外的任何指针 / 引用。
- `reset()` 函数体内必须显式调用 `m_countdownTimer->stop();`。
- 析构无需手动处理 `m_countdownTimer`（由 Qt 父子机制释放）。

## 验收标准

- 新增/扩展测试 `tests/test_game_controller.cpp`（**若 T-09 已落盘则向其追加，否则本卡需要新建该测试文件，包含 main 入口与 QCoreApplication 实例**）：
  - 用例 A：`startGame() → handleReadyKey() → reset()` 后，`state() == Idle`；继续 `processEvents()` 推进 ≥ 3 秒，`countdownTick` 信号 **再也不发**。
  - 用例 B：连续 `handleReadyKey()` × 3，倒计时不会叠加（验收：观察 `countdownTick` 在 3 秒内总发射次数 ≤ 4，即不重复 3-2-1-0 多轮）。
- 手工冒烟：进入单人模式后立即按 ESC 返回菜单 → **重复 10 次**，无崩溃，无日志中遗留 timer 调用。

## 不允许做的事

- 不允许把倒计时拆成新组件。
- 不允许引入 `QStateMachine`。
- 不允许把倒计时间隔（700ms）作为本卡的可调参数，**保持现状**。

## 反问提示

- 若 T-09 尚未落盘且该卡的新测试与 T-09 计划重叠，**反问主程**是否合并到 T-09。
- 若发现 `QCoreApplication` 在测试入口的处理方式与既有 `tests/CMakeLists.txt` 不兼容，**反问主程**。
