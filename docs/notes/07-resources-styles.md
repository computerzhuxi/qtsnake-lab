# 里程碑 7：resources 资源与样式

## 做什么

定义全局 QSS 样式表并通过 Qt .qrc 资源文件打包进可执行文件。

## 怎么做

- `main.qss` 统一管理所有控件的暗色电竞风格：按钮（暗底+霓虹绿悬停）、标签、输入框、浮层面板
- `resources.qrc` 将 QSS 文件纳入 Qt 资源系统，编译时嵌入 exe
- main.cpp 启动时通过 `qApp->setStyleSheet()` 全局加载
- 特殊控件通过 objectName 选择器区分：`#btnPrimary`(绿色高亮)、`#btnDanger`(红色危险)、`#titleLabel`、`#scoreLabel` 等

## 为什么这样做

- **全局样式表而非逐个 widget setStyleSheet**：修改风格只需改 .qss 文件，所有 widget 自动一致
- **.qrc 打包**：QSS 编译进 exe，发布时不依赖外部文件
- **objectName 做差异化**：避免写多份 .qss 文件，维护在同一个文件内更清晰
