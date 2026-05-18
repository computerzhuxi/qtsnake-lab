# T-06 — 文档澄清"死亡帧渲染策略"

> Phase-2 · 改动幅度：小（仅文档 + 可选轻量测试） · 串行序号：6

## 任务描述

把 "撞墙后玩家屏幕停留在撞墙前最后合法位置" 的 UX 策略 **显式写进文档与头文件注释**，避免后来者误判为缺陷。

> 背景：当前 `update()` 序列把 `RenderComponent` 放在首位，渲染上一 tick 结束态；导致碰撞那一 tick **不再渲染**，玩家停留在撞墙前合法位置。这是**有意为之**的 UX。

## 输入约束

- 可修改文件，仅限：
  - `docs/architecture.md`
  - `docs/design.md`
  - `src/controller/components/RenderComponent.h`（仅头注释）
- 可选新建文件（**仅当本卡决定加入回归测试**）：
  - `tests/test_render_strategy.cpp` 或追加到 `tests/test_components.cpp`
- 可选修改文件（**仅当上一项执行**）：
  - `src/controller/components/RenderComponent.h/cpp`：抽 `IRenderSink`/`RenderSink` 接口便于测试 spy（需反问主程是否允许此重构）。

## 输出约束

### 必做（文档）

- `docs/architecture.md` 中原 "Render 最先执行" 段落整段替换为如下表述（**字面意义一致即可，标题可微调**）：

> **死亡帧策略**：`RenderComponent` 在 `update()` 序列首位执行，渲染的是 "上一 tick 结束态"。当某帧发生碰撞（`MoveComponent` 已把头推入非法格、`CollisionComponent` 标记 `gameOver`），该帧不会再被渲染一次，因此 **玩家屏幕停留在撞墙前的最后合法位置**。这是有意为之的 UX，避免显示蛇头与墙体重合的画面。

- `docs/design.md` 在 4.4 节末尾追加同语义一句话备注。
- `src/controller/components/RenderComponent.h` 头注释（`/// \brief`、`/// \details`）增加一行：
  > 调用时机：每 tick 起始，渲染上一帧结束态；碰撞帧不再次渲染，玩家停留在撞墙前合法位置。

### 可选（测试，需主程批准后才执行）

- 引入轻量 spy（如 `RenderSpy : public RenderComponent` 或抽接口），断言：
  - 模拟 "蛇朝墙移动 → tick × N 直到 gameOver"，统计 `RenderComponent::update` 被以 `state.gameOver == true` 状态调用的次数为 **0**。

## 验收标准

- `architecture.md`、`design.md`、`RenderComponent.h` 三处表述一致、互无矛盾。
- 不影响既有测试。
- 若引入 spy 测试，则该用例通过；否则在 PR 描述中说明 "降级为文档评审"。

## 不允许做的事

- 不允许修改 `GameController::update()` 调用顺序。
- 不允许新增 `State`。
- 不允许引入异步 / 延迟渲染。

## 反问提示

- 决定是否引入 spy 测试 **必须先反问主程**——这是可选项，默认 **不引入**。
