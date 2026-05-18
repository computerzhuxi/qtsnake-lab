# 开发 Agent 执行守则

> 适用范围：所有承接 Snake Arena 项目 Task Card 的开发 Agent / 开发者。  
> 主程（Tech Lead）签发：v1，锁定。  
> 违反守则的提交一律被打回，**不视作工作量**。

---

## 1. 总则

1. **一次只领一张 Task Card**。完成、提交、通过审查之后才能领下一张。
2. **Task Card 是合同**。任务描述 / 输入约束 / 输出约束 / 验收标准 / 不允许做的事——五项字段中**任一字面之外**的判断点都视为"不确定"。
3. 遇任何不确定 → **立即停手，反问主程**。禁止猜测、禁止"顺手扩展"、禁止"反正都要改"。

---

## 2. 范围红线

1. 改动文件清单必须 **完全落在** Task Card "输入约束" 内。任一文件越界 → 一律打回，无例外。
2. 不得修改 Task Card 未授权的模块，即使发现"明显的 bug"——**记录后反问主程**，不得自行修复。
3. 不得新增第三方依赖（`FetchContent` / `find_package` 之外）。
4. 不得引入新模块、新单例、新全局变量。
5. 不得修改 `docs/AGENTS.md`、`docs/plans/`、`docs/tasks/` 下任何文件——它们由主程维护。

---

## 3. 接口红线

1. 修改 / 新增的 API 签名必须**与 Task Card "输出约束" 字面一致**（参数顺序、类型、可见性、`const`、引用 / 指针）。
2. 不得借机调整无关 API 的可见性、名字、命名空间。
3. 不得为了测试而暴露生产代码的私有成员（无 `friend`，无 "test-only getter"）。

---

## 4. 测试红线

1. 新增 / 受影响的测试必须在 PR 中跑通；不得 `DISABLED_` 标记跳过。
2. 不得使用 `sleep` / `this_thread::sleep_for` 推进时间——一律 Qt 事件循环。
3. 不得引入 `QtTest` / `gmock`（项目仅用 gtest）。
4. 测试运行总时长不得超过 30 秒（CI 友好）。
5. 行覆盖率：**非减**。新增模块的行覆盖率要求见各 Task Card。

---

## 5. 风格红线

1. 注释使用 `/// \brief` / `/// \details` 格式，与项目既有头文件一致。
2. C++17 范围 for / `auto` / `const&` 使用恰当；不得引入 C++20 特性。
3. 不得在 `src/app/` 下使用 `setStyleSheet`（QSS 集中于 `main.qss`）。
4. 日志统一走 `LOG_INFO/WARN/ERROR` 宏；不另起 `std::cout` / `qDebug` 通道。
5. 不得使用 `using namespace std;`。

---

## 6. 内存与生命周期红线

1. `src/app/` 内不得出现裸 `new` / `delete`。所有 widget 走 Qt 父子机制，所有 controller / service 走 `unique_ptr`（不传 Qt parent）。
2. 不得对 Qt 父子托管的对象 `delete`。
3. 不得让 lambda 捕获将被 `delete` 的指针 / `unique_ptr` 内部对象。
4. 不得在头文件中放置非内联的非模板函数定义。

---

## 7. 提交与反馈格式

每张 Task Card 完成提交时，必须附以下三段：

### 7.1 改动文件清单

```
+ src/app/AppShell.h         (modified)
+ src/app/AppShell.cpp       (modified)
- src/app/...                (deleted)
```

### 7.2 新增 / 修改测试列表

```
tests/test_game_controller.cpp::idleToReady (new, pass)
tests/test_components.cpp::FoodComponent.* (adapted, pass)
```

### 7.3 一句话冒烟结论

```
手工跑了 "主菜单 → 单人 → ESC × 5"，5/5 通过，无崩溃，日志无 WARN。
```

---

## 8. 反问触发场景（必停手）

遇到以下任一情况，**立即停止编码，向主程发起反问**，附"我看到的现象 + 我的两个候选方案"：

1. Task Card "输入约束" 不允许修改的文件，**但必须改它**才能完成验收。
2. 编译错误指向 Task Card 未授权的模块。
3. 既有测试用例与本卡的"输出约束"冲突。
4. 既有代码风格与本卡的"不允许做的事"冲突。
5. Qt / CMake / FetchContent 配置问题超出 Task Card 描述。
6. 发现影响范围超出本卡的 bug（**记录、反问，不修复**）。

---

## 9. 终止条件

任一以下情况，**主程有权立刻终止你的代码、撤回任务卡**，重新分配或重写：

- 越界修改且未事先反问。
- 用 `sleep` 等捷径绕过测试约束。
- 提交时未附"改动文件清单 / 测试列表 / 冒烟结论"。
- 引入未经批准的依赖、单例、全局状态。

---

## 10. 主程联系点

- 设计变更请求 → 通过对话发起 "T-XX 设计变更：…"。
- 任务卡歧义 → 通过对话发起 "T-XX 反问：…"。
- 进度报告 → 通过对话发起 "T-XX 完成：…" 并附 §7 三段。

---

## 11. Git 工作流（从 T-03 起强制执行）

### 11.1 提交粒度

- **一张 Task Card = 一个 git commit**。不得把多张卡的改动合并到同一 commit。
- 不得在工作树里同时存在多张未完成卡的混合 diff。开始下一张卡前必须先把上一张提交（或主动 stash）。

### 11.2 提交时机

- 开发 Agent **不直接 commit**——按 AGENTS §7 三段格式提交完整报告即可。
- 主程在审查通过后，由主程执行 `git add` + `git commit`。
- 不再向 `docs/progress.md` 回填 commit 短 hash——`git log` 即真实来源；commit message footer 已含 `任务卡:` 与 `审查通过:` 元数据，`git log --grep="T-XX"` 一行即可检索。
- 提交者身份：仓库 `git config user.name/email` 已配置；主程不修改 git config。

### 11.3 Commit Message 格式

```
<type>(T-XX): <一句话主旨，<= 50 字>

<空行>

<正文：3-6 行，回答"为什么"，链向 docs/tasks/T-XX-*.md>

<空行>

任务卡: docs/tasks/T-XX-*.md
审查通过: YYYY-MM-DD
```

允许的 `<type>`：
- `feat` 新功能 / 新模块
- `fix` 缺陷修复（含 hotfix 卡 T-0a / T-0b）
- `refactor` 重构（行为不变）
- `test` 仅新增/调整测试
- `docs` 仅文档
- `chore` 构建 / 工具链 / 配置
- `perf` 性能优化

### 11.4 不允许的操作

- 不允许 `git push --force` 到 main / master。
- 不允许 `git commit --amend` 已推送到远端的提交。
- 不允许在 commit message 中放置 secrets（密钥、token、密码）。
- 不允许跳过 hooks（`--no-verify`）。
- 不允许在 commit message 中放置 emoji 或营销话术。

### 11.5 历史保护

- Phase-2 之前的历史不动。
- T-03 起的每一个 Task Card 必须对应一个独立 commit，便于面试时按 commit 走读项目演进。
