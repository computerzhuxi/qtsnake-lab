# T-05 — 统一蛇方向防护，移除双重逻辑

> Phase-2 · 改动幅度：极小 · 串行序号：5

## 任务描述

约定 **单一防护点**——只有 `Snake::setDirection` 负责 180° 反转检查；`InputComponent::setDirection` 改为单纯缓存。

## 输入约束

- 可修改文件，仅限：
  - `src/controller/components/InputComponent.h`
  - `src/controller/components/InputComponent.cpp`
  - `tests/test_components.cpp`（追加测试）
- **不得修改 `Snake.h` / `Snake.cpp`。**
- 不得修改其他模块。

## 输出约束

- `InputComponent::setDirection(Direction)` 函数体改为单行：`m_direction = dir;`。
- `InputComponent.h` 类注释更新，明确：
  > 仅缓存最新方向输入；180° 反转防护由 `Snake::setDirection` 在 `update()` 阶段统一执行。

## 验收标准

- 新增测试（在 `tests/test_components.cpp` 追加）：
  - 用例 A：`InputComponent in; in.setDirection(Up); in.setDirection(Down); EXPECT_EQ(in 缓存的方向, Down);`（**通过 update 写入一条 Right 朝向的蛇并读回蛇方向验证**，因为 `m_direction` 是私有的，断言走 `Snake::direction()`）。
  - 用例 B：构造 `Snake s(Point{5,5}, Direction::Up);` → `in.setDirection(Down); in.update(state)` → `EXPECT_EQ(state.snakes[0].direction(), Direction::Up);`（验证 Snake 层防护仍生效）。
- 既有所有测试通过。

## 不允许做的事

- 不允许删除或修改 `Snake::setDirection` 中的反转检查（**这是单一防护点**）。
- 不允许引入新的"上一次提交方向"状态。

## 反问提示

- 若发现既有测试用例隐式依赖 `InputComponent` 的防护（例如直接读 `m_direction`），**反问主程**是否暴露 `direction() const` 只读访问器（默认答案：**不暴露**，断言走 Snake）。
