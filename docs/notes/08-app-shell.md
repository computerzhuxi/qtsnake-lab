# 里程碑 8：app 应用外壳

## 做什么

实现游戏底层 + 浮层叠加架构：游戏画面始终在 QGraphicsView 底层，所有界面（主菜单、暂停、结算、设置）作为 QWidget 浮层覆盖上方。

## 怎么做

- AppShell：顶层 QWidget，持有 GameView（底层）+ 四个浮层 widget。管理 show/hide 切换和键盘事件分发
- 浮层使用 QSS 的 `#overlay` 样式——半透明黑底遮罩，居中弹窗
- 主菜单：标题 + 四模式按钮 + 设置/退出链接
- 暂停：继续/重新开始/设置/返回主菜单
- 结算：分数+统计+再来一局/保存回放/返回主菜单
- 设置：QTabWidget 分操作标签（键位/语言）和游戏标签（速度/大小/音量）
- 键盘事件在 AppShell::keyPressEvent 中根据 GameController 状态分发：Ready→触发倒计时，Playing→方向键/ESC，Paused→ESC 恢复

## 为什么这样做

- **游戏底层 + 浮层叠加而非 QStackedWidget 翻页**：游戏画面无需重建，菜单切换自然，视觉效果更像游戏
- **AppShell 集中管理事件分发**：避免各浮层各自监听键盘，状态判断在一处完成
- **浮层 setGeometry 覆盖全窗口**：简单的绝对定位，无需复杂布局管理
