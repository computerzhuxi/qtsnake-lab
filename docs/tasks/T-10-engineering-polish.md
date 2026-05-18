# T-10 — 工程门面：README + SnakeServer 占位 + 编译警告升级

> Phase-2 · 改动幅度：中 · 串行序号：10（收尾）

## 任务描述

补齐面向 "面试作品集" 的工程门面：

1. 顶层 `README.md`（中英双段）。
2. `SnakeServer.exe` 占位 target（空壳 main，仅打 log）。
3. 严格编译警告（MSVC `/W4`、GCC/Clang `-Wall -Wextra`）。
4. 归档 `docs/plan.md` → `docs/archive/plan-v1.md`，并在 `docs/design.md` 顶部声明 "设计单源"。

## 输入约束

- 新建文件，仅限：
  - `README.md`
  - `src/main_server.cpp`
  - `docs/archive/plan-v1.md`（即 `docs/plan.md` 的移动副本）
  - `docs/screenshots/.gitkeep`（占位）
- 可修改文件，仅限：
  - 顶层 `CMakeLists.txt`
  - `docs/design.md`（**仅顶部追加一行声明**，不动正文）
- 删除文件，仅限：
  - `docs/plan.md`（**仅在确认归档副本已落盘后**）

## 输出约束

### README.md

- 中文段在前、英文段在后（或反之，保持双语对照）。
- 必含小节（中英文各一份）：
  - 项目简介（一句话）
  - 截图占位（链接 `docs/screenshots/`，描述待填）
  - 模块清单表（直接复用 `docs/architecture.md` 中的表）
  - 构建命令（CMake + Qt5；Windows MSVC + Linux 各一组）
  - Phase 路线图（引用 `docs/plans/phase-2-consolidation.md` 第 7 节）
  - 许可证（写 "MIT / TBD"）

### SnakeServer 占位

- `src/main_server.cpp` 内容仅：
  - 包含 `<QCoreApplication>` 与 `Logger.h`
  - `int main(int argc, char** argv)`
  - 函数体：构造 `QCoreApplication app(argc, argv);` → `LOG_INFO("SnakeServer", "SnakeServer not implemented yet");` → `return 0;`（**不 exec 事件循环**，立即返回）
- 顶层 `CMakeLists.txt` 新增：

```cmake
add_executable(SnakeServer src/main_server.cpp)
target_link_libraries(SnakeServer PRIVATE snake_core snake_logging Qt5::Core)
```

- **不得** 链接 `Qt5::Widgets`、`snake_ui`、`snake_app`、`snake_controller`、`snake_audio`。

### 编译警告

- 顶层 `CMakeLists.txt` 在 `project(...)` 之后追加：

```cmake
if(MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()
```

- **不加** `/WX` 或 `-Werror`（避免阻塞 CI）。
- 升级警告后若既有代码出现新的警告：
  - **逐一评估**：能在不破坏其他模块约束的前提下修复则修复（限 `src/` 范围，与本卡同 PR 提交）。
  - 若涉及第三方 / FetchContent 来的 googletest 警告，**反问主程**是否抑制（默认答案：**抑制**，可对 googletest 目标 `target_compile_options(... PRIVATE /W0)` 之类）。

### 文档归档

- `docs/plan.md` 整体内容复制到 `docs/archive/plan-v1.md`。
- 在 `docs/archive/plan-v1.md` 顶部追加 1 行：

> 本文件为 `docs/plan.md` 的归档版本（v1）；最新设计请见 `docs/design.md` 与 `docs/plans/`。

- 删除原 `docs/plan.md`。
- 在 `docs/design.md` 顶部第 1 行（**# 标题前**）追加：

> 本文件为 Snake Arena 项目的设计单一来源（Single Source of Truth）。历史 `docs/plan.md` 已归档至 `docs/archive/plan-v1.md`。

## 验收标准

- `cmake --build` 同时产出 `SnakeArena.exe` 与 `SnakeServer.exe`。
- `SnakeServer.exe` 运行后立刻退出（exit code 0），控制台/日志可见 `"SnakeServer not implemented yet"`。
- 编译警告数：**不增加**（基线为本卡之前的版本，可在 PR 描述附构建日志摘要）。
- `docs/plan.md` 不再存在；`docs/archive/plan-v1.md` 内容与原 `plan.md` 一致（+顶部归档声明）。
- `README.md` 在 GitHub 渲染检查（人工或 `grip` 等）下排版正常。

## 不允许做的事

- 不允许在 `main_server.cpp` 中引入网络 / 业务逻辑。
- 不允许动 `docs/design.md` 正文。
- 不允许把 `/WX` 或 `-Werror` 打开。
- 不允许在 README 中放虚构的截图链接（用占位）。

## 反问提示

- 若构建系统在追加 `/W4` 后立刻在 Qt 自身头文件产生大量警告，**反问主程**是否对 Qt 目标 `SYSTEM` 包含路径化处理。
- 若 `Qt5::Core` 在当前 `find_package` 配置中未被显式启用，**反问主程**（看顶层 CMake 现有 `COMPONENTS` 列表已包含 `Widgets Network LinguistTools`，理论上 `Core` 隐式可用）。
