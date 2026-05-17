# 里程碑 10：QSS 全局样式

## 做什么

定义暗色电竞风格全局样式表，通过 Qt .qrc 打包进可执行文件。

## 怎么做

- `main.qss`：全局按钮（暗底+霓虹绿悬停）、标签、输入框、浮层面板样式
- `.qrc` 将 QSS 打入 Qt 资源系统
- main.cpp 启动时 `qApp->setStyleSheet()` 全局加载
- 特殊控件通过 objectName 区分：`#btnPrimary`(绿色高亮)、`#btnDanger`(红色危险)、`#titleLabel`、`#scoreLabel`

## 为什么这样做

- **全局样式表**：修改风格只改 .qss，所有 widget 自动一致
- **.qrc 打包**：QSS 编译进 exe，发布时不依赖外部文件
