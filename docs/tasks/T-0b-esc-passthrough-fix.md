# T-0b — 热修复：AppShell::keyPressEvent ESC 直通穿透（BUG-1）

> Phase-2 · 改动幅度：极小 · 串行序号：1.2（紧急插入，T-01 之后、T-0a 之前）  
> 触发原因：用户冒烟时观察到日志显示按下 ESC 后状态机先 `Playing → Paused`，紧接着 `Paused → Playing`——一次按键内完成 pause + resume。  
> 阻塞性：阻塞所有后续手工冒烟（玩家无法真正进入暂停状态）。**先于 T-0a 处理**。

## 任务描述

修复 `AppShell::keyPressEvent` 中 ESC 路径在一次按键事件内同时进入"Playing 分支 (pause)"与"Paused 分支 (resume)"的逻辑缺陷。

## 根因（已由主程诊断）

`keyPressEvent` 当前结构：

```cpp
if (state == Playing) {
    switch (key) {
        ...
        case Escape:
            pause();       // state 切到 Paused
            showPause();
            break;          // 注意：break 只跳出 switch，未跳出函数
    }
}

if (state == Paused) {     // 上面刚切成 Paused，这里立即为 true
    if (key == Escape) {
        resume();           // 又切回 Playing
        hideAllOverlays();
    }
}
```

两个 `if` 块顺序执行，且都用"当前实时状态"判定 → ESC 在 Playing 与 Paused 间瞬时往返。

## 输入约束

- 可修改文件，仅限：
  - `src/app/AppShell.cpp`
- **不得修改任何其他文件**（包括 `AppShell.h`、`GameController` 任何文件、`tests/`、`docs/`、QSS、`PauseWidget` 等）。
- 不得新建文件。

## 输出约束

### 修复方案（强制使用方案 A，方案 B 仅在反问获得主程明确批准后采用）

**方案 A（默认必选，最小改动）**：

在 `AppShell::keyPressEvent` **函数开头** 取一次状态快照，**两个 `if` 块都使用快照而非实时状态**。等价代码骨架（**示意，不直接抄录**）：

```cpp
void AppShell::keyPressEvent(QKeyEvent* event) {
    if (!m_controller) { QWidget::keyPressEvent(event); return; }

    const auto state = m_controller->state();   // ← 新增此行，作为本次事件的状态快照

    if (state == GameController::State::Ready) { ... }

    if (state == GameController::State::Playing) {
        // ESC 分支保持原逻辑（pause + showPause），不需要 return
    }

    if (state == GameController::State::Paused) {  // ← 改用快照
        if (event->key() == Qt::Key_Escape) { resume + hideAllOverlays }
    }

    QWidget::keyPressEvent(event);
}
```

**关键点**：所有对 `m_controller->state()` 的多次调用替换为 `state` 局部变量；两个 `if` 块的判定都基于"按键到达时的初始状态"。

**方案 B（仅反问后允许）**：

把整段 `keyPressEvent` 用 `switch(state)` 重构。**默认不采用此方案**，因超出热修复范围。

### 不允许的修复方式

- 不允许在 Playing/Escape 分支末尾直接 `return;` —— 表面也能跑通，但破坏了"事件函数末尾统一调用 `QWidget::keyPressEvent(event);"`的对称性，且后续维护时再加按键路径会立刻踩坑。
- 不允许把 `pause()` 改成不发 `stateChanged` 信号或不更新 `m_phase` 来 workaround。
- 不允许动 `GameController::pause` / `resume` 的语义。
- 不允许把 `if` 改成 `else if` 形式（结构上等价于本卡的快照方案，但语义不够明确；保留主程对最终结构的统一）。

## 验收标准

- 编译通过；既有所有测试通过；无新警告。
- 手工冒烟（**逐项确认，写入提交报告**）：
  1. 进入单人模式，倒计时结束后游戏开始（Playing）。
  2. 按 ESC：日志中只看到 `State: Playing -> Paused` **一条**记录，**不**出现紧跟的 `Paused -> Playing`。
  3. 在 Paused 状态再按 ESC：日志显示 `State: Paused -> Playing`，游戏恢复。
  4. 重复 5 次"Playing ⇄ Paused"切换，状态机日志符合预期。
- 静态扫描：`AppShell::keyPressEvent` 函数内**仅有一处** `m_controller->state()` 调用（位于函数顶部的快照赋值）。

## 不允许做的事

- 不允许修改 `AppShell.h`（包括成员变量、方法签名、注释）。
- 不允许借机修复 `keyPressEvent` 中其他任何不优雅之处（如重复 `event->key()` 查询、按键路径合并等）。
- 不允许加入新的 Qt 事件过滤器或快捷键体系（QShortcut 等）。
- 不允许引入 mutex / atomic（状态机已在主线程）。

## 反问触发场景

- 若编译时发现 `m_controller->state()` 的返回类型与 `auto` 推导冲突 → **反问主程**（默认答案：直接写 `GameController::State state = m_controller->state();`）。
- 若发现 Ready 分支末尾的 `return;` 会被新的快照逻辑误绕过 → **反问主程**（默认答案：保留 Ready 分支末尾的 `return;` 不动，那里语义独立）。
- 若想顺手把方案 B（switch 重构）做了 → **反问主程**，禁止自行扩展。

## 提交报告（按 AGENTS.md §7 三段格式）

```
## 改动文件清单
+ src/app/AppShell.cpp (modified)

## 新增 / 修改测试列表
（无；BUG-1 的回归测试将在 T-09 的状态机测试套件中覆盖）

## 一句话冒烟结论
进入单人 → ESC，日志仅出现 "State: Playing -> Paused" 一条；再按 ESC 恢复；
Playing ⇄ Paused 切换 5 次，状态机日志全部符合预期；
（PauseWidget 浮层视觉问题留待 T-0a 修复 QSS 后才能验证视觉效果）。
```
