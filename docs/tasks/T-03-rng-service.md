# T-03 — 引入 `RngService`，集中随机性

> Phase-2 · 改动幅度：中 · 串行序号：3 · 前置：T-02

## 任务描述

把所有随机源（spawn 选位 + 食物生成）统一到一个 **可注入种子** 的 `RngService`，为 Phase-3 AI / Phase-5 回放的确定性铺路。

## 输入约束

- 新建文件，仅限：
  - `src/core/RngService.h`（**仅头文件实现**，避免增加翻译单元）。
- 可修改文件，仅限：
  - `src/core/CMakeLists.txt`（仅在头文件依赖列表中追加 `RngService.h`）。
  - `src/controller/components/FoodComponent.h`
  - `src/controller/components/FoodComponent.cpp`
  - `src/controller/GameController.h`
  - `src/controller/GameController.cpp`
  - `tests/test_components.cpp`（用例适配 + RngService 确定性测试）
  - `tests/test_game_controller.cpp`（**追加** GameController.setSeed 确定性测试；T-02 已建立此文件）

> 修订记录：v0.2（2026-05-18）补加 `tests/test_game_controller.cpp` 到授权清单。原 v0.1 漏列，与"验收标准"要求新增 GameController 确定性测试不闭合，主程补正。
- **不得修改其他模块**（ui / app / logging / audio）。

## 输出约束

### `RngService`

- 类位于头文件 `src/core/RngService.h`，`#pragma once` 或传统 include guard 任选其一与项目风格一致。
- 接口（最少集）：
  - `class RngService { ... }`
  - `void seed(unsigned s);`（`s == 0` 时用 `std::random_device` 真随机）
  - `int intInRange(int loInclusive, int hiInclusive);`
  - `std::mt19937& engine();`（供高阶用法，但本 Phase 不得在外部使用 distribution）
- 内部持有 `std::mt19937 m_engine;`；不得有静态全局状态；不得是单例。

### `FoodComponent`

- 构造函数签名变更为 `FoodComponent(RngService* rng);`（裸指针，生命周期由外部 GameController 管理）。
- **删除** `setSeed()` 接口。
- **删除** 私有 `std::mt19937 m_rng;`。
- `spawnFood()` 内部用 `rng->intInRange(0, freeCells.size() - 1)` 选位。

### `GameController`

- 新增私有成员 `std::unique_ptr<RngService> m_rng;`，在构造函数中 `std::make_unique<RngService>()`。
- 新增公共方法 `void setSeed(unsigned s);`：
  - 仅当 `m_phase == State::Idle` 时执行 `m_rng->seed(s)` 并 `LOG_INFO`。
  - 其他状态打 `LOG_WARN("GameController", "setSeed ignored: not in Idle")` 并 `return`。
- 构造 `FoodComponent` 时传入 `m_rng.get()`。
- `startGame()` 中 spawn 位置选择：用 `m_rng->intInRange(0, w-1)` / `(0, h-1)` 代替原 `std::random_device + uniform_int_distribution`。**删除** `startGame()` 内对 `<random>` 的直接使用。

## 验收标准

- 编译通过；既有所有测试通过。
- 新增测试（追加到 `tests/test_components.cpp` 或新建 `tests/test_rng_determinism.cpp`，前者优先）：
  - 用例：`RngService a, b; a.seed(42); b.seed(42); for(10) EXPECT_EQ(a.intInRange(0,99), b.intInRange(0,99));`。
- 新增测试（用 `GameController + setSeed`）：
  - 用例：同一种子构造两个 `GameController` 并 `startGame()`，**前 10 颗食物坐标完全一致**。
  - 实现提示：可在测试中 `controller.setSeed(42)` → `startGame()` → 多次手动触发 `update()` 边界（但本卡**不要求**真跑 QTimer，模拟即可；若需 Qt 事件循环帮助，反问主程）。
- 静态扫描：`grep -r "#include <random>" src/` 仅命中 `src/core/RngService.h`，其余源文件清零。

## 不允许做的事

- 不允许把 `RngService` 做成单例（无 `instance()`、无全局变量）。
- 不允许在 `core/` 外再 `#include <random>`。
- 不允许给 `RngService` 加 setter 之外的可变状态接口。
- 不允许动 `Snake` / `Board` / `Point`。

## 反问提示

- 若 `tests/test_components.cpp` 既有 `FoodComponent food;` 默认构造的用法在新签名下编译错误，**反问主程**是否提供默认构造（默认答案：**不提供**，调用方必须显式提供 `RngService`）。
- 若 spawn 位置随机的 "起始方向选择" 逻辑也需要可重现，**反问主程**是否一并改用 RngService（默认答案：**是**，所有 startGame 内部随机均通过 `m_rng`）。
