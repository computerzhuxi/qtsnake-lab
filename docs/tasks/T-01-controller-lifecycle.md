# T-01 — 修复 AppShell / GameController 生命周期

> Phase-2 · 改动幅度：中 · 串行序号：1

## 任务描述

消除 `AppShell` 中所有 `delete m_controller` 调用与裸指针缓存，统一为 `std::unique_ptr<GameController>` 管理，且 **不传 `QObject` parent**（避免 Qt 父子机制与 unique_ptr 双重释放）。

## 输入约束

- 可修改文件，仅限：
  - `src/app/AppShell.h`
  - `src/app/AppShell.cpp`
  - `src/controller/GameController.h`（仅必要时，仅改构造签名）
  - `src/controller/GameController.cpp`（仅必要时，仅改构造函数体）
- **不得新建任何文件。**
- **不得修改其他模块**（core / ui / tests）。

## 输出约束

- `AppShell::m_controller` 类型变为 `std::unique_ptr<GameController>`。
- `AppShell::m_inputComponent` 成员 **删除**；所有使用处改为 `m_controller->input()`。
- 所有 `m_controller = new GameController(scene, this)` 改为 `m_controller = std::make_unique<GameController>(scene)`。
- `GameController` 构造函数：移除 `QObject* parent` 形参，或保留但默认 `nullptr` 且 `AppShell` 一律传 `nullptr`（**约定使用前者，签名变更**）。
- `AppShell::~AppShell()` **不再** 出现 `delete m_controller`。
- 所有 `connect(m_controller, ...)` 改为 `connect(m_controller.get(), ...)`。
- 切场景 / 重启场景中，控制器**仅通过** `m_controller.reset(...)` 释放与重建。
- `m_controller->input()` 不得在 `m_controller == nullptr` 时被调用（先判空）。

## 验收标准

- 编译通过；既有 `tests/` 全部通过；**无新增编译警告**。
- 手工冒烟（在 PR 描述中描述结果）：
  - 主菜单 → 单人 → ESC → 返回主菜单 → 再开单人，**连续 5 次无崩溃**。
  - 单人游戏中撞墙 → 结算 → 再来一局 → 再撞墙，**连续 5 次无崩溃**。
- 静态扫描：`AppShell.cpp` 中 `new GameController` 与 `delete m_controller` 出现次数均为 **0**。

## 不允许做的事

- 不允许同时使用 `unique_ptr` **与** Qt parent（已约定仅 unique_ptr）。
- 不允许扩展到其他模块。
- 不允许借机调整任何信号槽连接的业务逻辑（仅做 `.get()` 形式调整）。
- 不允许新增 `friend` 关系。

## 反问提示

任一不确定 → 停手反问主程：

- 若 `GameController` 构造签名变更导致 `tests/` 出现编译错误，**立刻反问**（不要自行改 tests）。
- 若发现某处 `connect` 涉及 lambda 捕获 `m_controller`，**立刻反问**（避免捕获原始指针）。
