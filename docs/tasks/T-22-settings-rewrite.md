# T-22：SettingsWidget 面板化 + Tab 重构 + QSS 收尾

> 签发：主程 | 日期：2026-05-21 | 状态：待领取
> 分支：`phase-3-collision-refactor`
> 前置卡：T-21（ShineLabel 已通过）

---

## 任务描述

将 SettingsWidget 修至与 `docs/prototypes/ui-mockup-v4.html` Settings 状态一致：
1. 添加 panel 面板包裹层（与 T-20 PauseWidget/GameOverWidget 同模式）
2. QTabWidget → 自定义 QPushButton 标签 + QStackedWidget
3. 表单控件 QSS（QComboBox / QSlider）

---

## 输入约束

### 允许修改/新增的文件

```
~ src/app/SettingsWidget.h
~ src/app/SettingsWidget.cpp
~ src/resources/style/main.qss
```

### 不允许做的事

- 不得修改 core / controller / ui 层任何文件
- 不得在 `src/app/` 中使用 `setStyleSheet`
- 不得修改 AppShell / GamePage（SettingsWidget 公开接口不变——仅 `backClicked()` 信号）
- 不得使用 `using namespace std;`
- 不得使用 QTabWidget（改为 QPushButton + QStackedWidget）

---

## 输出约束

### 1. 结构：overlay → panel（与 T-20 同模式）

```
SettingsWidget (QWidget, objectName="overlay")
  QVBoxLayout
    addStretch()
    panelWidget (QWidget, objectName="settingsPanel")
      QVBoxLayout [margins=32,20,32,20, spacing=0]
        QLabel("设 置")         objectName="settingsTitle"  AlignCenter
        spacing(16)
        [tabLayout: QHBoxLayout 两个 QPushButton]
        spacing(16)
        QStackedWidget(m_stack)
          page 0: 操作
          page 1: 游戏
        spacing(16)
        QPushButton("返回")
    addStretch()
```

panel 宽度：`setMinimumWidth(360)`、`setMaximumWidth(480)`。外层 `addWidget(panel, 0, Qt::AlignCenter)`。

### 2. 自定义 Tab 按钮

两个 QPushButton，水平排列：

```cpp
auto* tabLayout = new QHBoxLayout;
tabLayout->setSpacing(0);

auto* btnControl = new QPushButton(tr("操作"), panel);
btnControl->setObjectName("settingsTab");
btnControl->setProperty("active", true);
btnControl->setFlat(true);

auto* btnGame = new QPushButton(tr("游戏"), panel);
btnGame->setObjectName("settingsTab");
btnGame->setProperty("active", false);
btnGame->setFlat(true);

tabLayout->addWidget(btnControl);
tabLayout->addWidget(btnGame);
```

点击切换逻辑：
```cpp
connect(btnControl, &QPushButton::clicked, [this, btnControl, btnGame]() {
    m_stack->setCurrentIndex(0);
    btnControl->setProperty("active", true);
    btnGame->setProperty("active", false);
    refreshStyle(btnControl);
    refreshStyle(btnGame);
});
// btnGame 同理，切到 index 1
```

`refreshStyle` 辅助函数：
```cpp
void refreshStyle(QWidget* w) {
    w->style()->unpolish(w);
    w->style()->polish(w);
}
```

### 3. Tab 内容（QStackedWidget 两页）

不再用 QTabWidget 的 tab 页面，直接创建两个独立 QWidget page。

**操作页（page 0）**：QVBoxLayout [spacing=8]
- QLabel("移动键位") objectName="settingsLabel"
- QComboBox [方向键 / WASD] objectName="settingsCombo"
- QLabel("语言 / Language") objectName="settingsLabel"
- QComboBox [中文 / English] objectName="settingsCombo"
- addStretch()

**游戏页（page 1）**：QVBoxLayout [spacing=8]
- QLabel("游戏速度") objectName="settingsLabel"
- QComboBox [慢(150ms) / 中(100ms) / 快(60ms)] objectName="settingsCombo"，默认选中"中"
- QLabel("棋盘大小") objectName="settingsLabel"
- QComboBox [小(15×15) / 中(20×20) / 大(30×30)] objectName="settingsCombo"，默认选中"中"
- QLabel("音量") objectName="settingsLabel"
- QHBoxLayout [QSlider + QLabel(值)] — slider 设 objectName="settingsSlider"、label 设 objectName="settingsSliderValue"
- addStretch()

QSlider：range(0, 100)，value(50)，horizontal。QLabel 初始显示 "50"。slider `valueChanged(int)` 信号更新 label 文本。

### 4. 标题样式

```css
QLabel#settingsTitle {
    color: #e0e0ff;
    font-size: 22px;
    font-family: monospace;
    letter-spacing: 6px;
}
```

### 5. main.qss 新增

```css
/* SettingsWidget */
QWidget#settingsPanel {
    background: #0a0a1a;
    border: 1px solid #1a1a4e;
    border-radius: 10px;
}
QLabel#settingsTitle {
    color: #e0e0ff;
    font-size: 22px;
    font-family: monospace;
    letter-spacing: 6px;
}

/* SettingsWidget — tab buttons */
QPushButton#settingsTab {
    background: transparent;
    border: none;
    border-bottom: 2px solid transparent;
    color: #556;
    padding: 8px 0;
    font-size: 13px;
    font-family: monospace;
}
QPushButton#settingsTab[active="true"] {
    color: #00ff88;
    border-bottom-color: #00ff88;
}

/* SettingsWidget — form controls */
QLabel#settingsLabel {
    color: #556;
    font-size: 10px;
    letter-spacing: 2px;
    margin-top: 12px;
    margin-bottom: 4px;
}
QComboBox#settingsCombo {
    background: #08081a;
    border: 1px solid #1a1a4e;
    border-radius: 4px;
    color: #0ff;
    padding: 7px 12px;
    font-size: 13px;
    font-family: monospace;
}
QComboBox#settingsCombo:focus {
    border-color: #00ff88;
}
QComboBox#settingsCombo QAbstractItemView {
    background: #0a0a1a;
    color: #ccc;
    selection-background-color: #1a1a4e;
}
QSlider#settingsSlider::groove:horizontal {
    height: 4px;
    background: #1a1a4e;
    border-radius: 2px;
}
QSlider#settingsSlider::handle:horizontal {
    width: 14px;
    height: 14px;
    margin: -5px 0;
    background: #00ff88;
    border-radius: 7px;
}
QLabel#settingsSliderValue {
    color: #00ff88;
    font-size: 13px;
    min-width: 24px;
}
```

---

## 验收标准

1. 编译 /W4 零警告
2. 45 测试全绿
3. SettingsWidget：半透明遮罩 + 居中 panel（圆角边框，360-480px 宽，双轴居中）
4. 自定义 Tab 按钮：选中项绿色下划线 + 文字，未选中灰色
5. QStackedWidget 页面切换正常（操作 ↔ 游戏）
6. QComboBox 暗色风格与 HTML 原型一致
7. QSlider 绿色圆形手柄 + 右侧数值实时更新
8. `src/app/` 中零 `setStyleSheet`
9. 目测与 `docs/prototypes/ui-mockup-v4.html` Settings 状态一致

---

## 提交格式

```
改动文件清单:
~ src/app/SettingsWidget.h
~ src/app/SettingsWidget.cpp
~ src/resources/style/main.qss

新增/修改测试列表:
无测试变更，45 全绿。

一句冒烟:
编译 /W4 零警告，45 测试全绿，src/app/ 零 setStyleSheet，SnakeArena.exe：
主菜单 → 设置 → panel 双轴居中（360-480px）+ 自定义 Tab 点击切换页面 +
下拉框暗色风格 + 滑块绿色手柄拖动数值实时更新 + 返回按钮可用，
目测与 ui-mockup-v4.html Settings 一致。
```
