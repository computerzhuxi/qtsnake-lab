# 里程碑 1：CMake 项目骨架

## 做什么

搭建 Snake Arena 的构建系统骨架。创建顶层 CMakeLists.txt + 10 个模块的 CMakeLists.txt，配置 Qt5 和 Google Test 依赖，确保能编译出空的静态库并通过 CI。

## 怎么做

- 顶层 CMake 设置 C++17、AUTOMOC、MSVC `/utf-8`（中文兼容）、Qt5 Widgets/Network/LinguistTools、Google Test (FetchContent v1.14.0)
- 每个模块目录下写 CMakeLists.txt，`add_library(... STATIC ...)` 编译为静态库
- 依赖关系：core 无依赖 → ui 依赖 core → controller 依赖 core + ui → app 依赖 controller + ui → main.cpp 链接所有库
- 每个模块先创建带 `/// \brief` 注释的 .h 和空的 .cpp 骨架
- tests/ 配置 Google Test 并创建占位测试 `EXPECT_TRUE(true)`

## 为什么这样做

- **静态库而非头文件库**：模块边界清晰，依赖方向强制在 CMake 层面检查，编译快
- **C++17**：Qt5 完全兼容，且支持 `if constexpr`、结构化绑定等现代特性
- **FetchContent 获取 Google Test**：不需要手动下载安装，cmake 配置时自动拉取，跨平台一致
- **占位 .cpp 先写好**：先跑通构建流水线，后续 TDD 只需改文件内容，不用动 CMake 结构
