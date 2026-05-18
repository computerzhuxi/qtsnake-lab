# T-0a — 热修复：CMakeLists 启用 AUTORCC，让 QSS 资源真正进入二进制

> Phase-2 · 改动幅度：极小 · 串行序号：1.5（紧急插入，T-01 之后、T-02 之前）  
> 触发原因：用户冒烟时发现 `[WARN] [main] Failed to load QSS stylesheet`，根因是 `CMAKE_AUTORCC` 未启用导致 `resources.qrc` 未编入 exe，进而暗色 TRON 样式完全失效、`PauseWidget` 等浮层背景透明。  
> 阻塞性：**阻塞 T-07**；不阻塞 T-02 ~ T-06、T-08 ~ T-09，但建议先做以恢复正确视觉基线，便于后续手工冒烟。

## 任务描述

修复构建管线，使 `src/resources/resources.qrc` 在构建时被 Qt rcc 编译进 `SnakeArena.exe`，使 `main.cpp` 中的 `QFile qss(":/style/main.qss"); qss.open(QFile::ReadOnly)` 在运行时成功返回 true，QSS 全局样式正常生效。

## 输入约束

- 可修改文件，仅限：
  - 顶层 `CMakeLists.txt`
- 不得新建任何文件。
- **不得修改 `src/resources/resources.qrc`**（其内容已正确）。
- **不得修改 `src/main.cpp`**（其加载逻辑已正确）。
- **不得修改 `src/resources/style/main.qss`**（其样式已正确）。
- 不得动其他模块。

## 输出约束

### 方案 A（首选，统一启用）

- 在顶层 `CMakeLists.txt` 中既有 `set(CMAKE_AUTOMOC ON)` 之后，追加一行：
  ```cmake
  set(CMAKE_AUTORCC ON)
  ```

### 方案 B（备选，仅当方案 A 在 CI/本机出现异常）

- 改用显式 `qt5_add_resources(...)`：
  ```cmake
  qt5_add_resources(SNAKE_RESOURCES_RCC ${RESOURCE_FILE})
  add_executable(SnakeArena
      src/main.cpp
      ${SNAKE_RESOURCES_RCC}
  )
  ```
- **二选一**，不得同时使用两种方式（会重复生成符号）。

## 验收标准

- `cmake --build` 后产生的 `SnakeArena.exe` 在运行时：
  - `[WARN] [main] Failed to load QSS stylesheet` **不再出现**。
  - 主菜单背景为暗色（`#06060f`），按钮符合 `main.qss` 描述的边框 / hover / pressed 状态。
  - 单人模式中按 ESC，**PauseWidget 出现为半透明深色蒙层 + 居中按钮组**（"继续游戏 / 重新开始 / 设置 / 返回主菜单"），视觉对应 `main.qss` 中 `#overlay` 与按钮规则。
- `cmake --build` 输出中可见 `AUTORCC: ...` 或 `Rcc...` 相关步骤（证据用于 PR 描述）。
- 编译警告数不增加。
- 既有单元测试不受影响。

## 不允许做的事

- 不允许把 QSS 内容内联进 C++ 源代码作为绕过手段。
- 不允许把 `.qss` 文件移到 exe 同级目录加载（必须走 `.qrc` 资源）。
- 不允许引入 `Qt6` / 其他版本切换。
- 不允许借机修改其他 CMake 行（如警告升级——那是 T-10 范围）。

## 反问触发场景

- 若启用 `CMAKE_AUTORCC` 后 `resources.qrc` 引发 "duplicate symbol" 或类似冲突（与既有 `${RESOURCE_FILE}` 加入 `add_executable` 双重处理冲突），**立即停手反问主程**：是否需要从 `add_executable` 的源列表里同时移除 `${RESOURCE_FILE}`（使 AUTORCC 单独处理）。
- 若发现 `find_package(Qt5 COMPONENTS ...)` 缺少 rcc 工具相关组件，**反问主程**。
- 若构建后样式仍未生效，需自检：`SnakeArena.exe` 体积是否较修复前增大若干 KB（QSS 进入了二进制）；如不增大，**反问主程** 取得诊断方向。

## 提交报告

按 `docs/AGENTS.md §7` 三段格式：

```
## 改动文件清单
+ CMakeLists.txt (modified, 1 line)

## 新增 / 修改测试列表
（无）

## 一句话冒烟结论
SnakeArena.exe 重新运行无 QSS WARN；主菜单背景暗色生效；ESC 暂停可见为半透明深色蒙层 + 按钮组。
```
