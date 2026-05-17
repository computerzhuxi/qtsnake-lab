# 里程碑 11：AppShell + 页面架构

## 做什么

主菜单和游戏界面分为两个独立页面（QStackedWidget），暂停/结算/设置/倒计时作为浮层覆盖在对应页面上。

## 窗口层级

```
AppShell (QWidget)
  └── QStackedWidget
        ├── Page 0: MenuPage           ← 纯菜单页面
        │     ├── 标题 + 蛇形装饰
        │     ├── 四个模式按钮
        │     └── 设置 / 退出 链接
        │
        └── Page 1: GamePage           ← 游戏页面
              ├── QGraphicsView         ← 底层
              ├── PauseWidget           ← 浮层
              ├── GameOverWidget        ← 浮层
              └── CountdownLabel        ← 浮层

  SettingsWidget（全局浮层，覆盖 QStackedWidget）
```

## 怎么做

- AppShell 持有 QStackedWidget + SettingsWidget
- MenuPage：独立 QWidget，主菜单 UI
- GamePage：QWidget 包含 QGraphicsView + 浮层
- Settings：全局 widget，覆盖在 QStackedWidget 上方
- 每个页面有 enter() / exit() 生命周期方法：
  - MenuPage::enter() / exit()：无需操作
  - GamePage::enter()：清场景、初始化控制器
  - GamePage::exit()：清场景、删除控制器

## 为什么这样做

- **页面生命周期**：enter/exit 确保每次进入都是干净状态，退出释放资源
- **页面分离**：主菜单和游戏互不干扰
- **浮层叠加**：暂停/结算浮在游戏上方
