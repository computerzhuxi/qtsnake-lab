# T-26：Settings 修改弹窗（"设置已更改，下局生效"）

> 签发：主程 | 日期：2026-05-21 | 状态：待领取
> 分支：`phase-3-collision-refactor`
> 前置卡：T-25（Settings 集成已通过）

---

## 任务描述

游戏中打开设置修改了参数，点"返回"时弹出确认弹窗，匹配 TRON 暗色风格：
- "设 置 已 更 改"
- "将在下一局生效"
- "重新开始"（绿色 primary） / "稍后再说"（暗色）

原型：`docs/prototypes/settings-dialog-mockup.html`

---

## 输入约束

### 允许修改/新增的文件

```
+ src/app/SettingsDialog.h     (新增：自定义 QDialog)
+ src/app/SettingsDialog.cpp   (新增：暗色 TRON 风格弹窗)
~ src/app/SettingsWidget.h     (新增 setGameSettings/settingsChanged + 脏检测)
~ src/app/SettingsWidget.cpp   (backClicked 逻辑改为脏检测)
~ src/app/AppShell.cpp         (connect settingsChanged + 响应)
~ src/app/CMakeLists.txt       (追加 SettingsDialog.cpp)
```

### 不允许做的事

- 不得修改 core / controller / ui 层
- 不得在 `src/app/` 中使用 `setStyleSheet`
- 不得使用 QMessageBox（样式不可控）
- 不得修改 GamePage
- 不得使用 `using namespace std;`

---

## 输出约束

### 1. SettingsDialog（自定义 QDialog）

```
SettingsDialog : QDialog (parent)
  setObjectName("settingsDialog")
  setFixedSize(340, 180)
  setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog)
  setModal(true)

  QVBoxLayout [margins=24,20,24,20, spacing=0, AlignCenter]
    QLabel("!")               objectName="dialogIcon"       AlignCenter
    spacing(10)
    QLabel("设 置 已 更 改")   objectName="dialogTitle"      AlignCenter
    spacing(6)
    QLabel("将在 下一局 生效")  objectName="dialogDesc"       AlignCenter
    spacing(18)
    QHBoxLayout [spacing=10]
      QPushButton("重新开始")   objectName="dialogPrimary"
      QPushButton("稍后再说")   objectName="dialogSecondary"

signals:
    void restartClicked();  // 用户选"重新开始"
    void laterClicked();    // 用户选"稍后再说"
```

构造中连接按钮信号：
- dialogPrimary clicked → emit restartClicked() + accept()
- dialogSecondary clicked → emit laterClicked() + reject()

### 2. QSS（main.qss 新增）

```css
/* SettingsDialog */
QDialog#settingsDialog {
    background: #0a0a1a;
    border: 1px solid #00ff88;
    border-radius: 8px;
}
QLabel#dialogIcon {
    color: #00ff88;
    font-size: 28px;
    font-family: monospace;
}
QLabel#dialogTitle {
    color: #e0e0ff;
    font-size: 15px;
    letter-spacing: 2px;
    font-family: monospace;
}
QLabel#dialogDesc {
    color: #556;
    font-size: 11px;
    font-family: monospace;
}
QPushButton#dialogPrimary {
    background: #0a0a1a;
    border: 1px solid #225544;
    border-radius: 5px;
    color: #00ff88;
    padding: 9px 0;
    font-size: 13px;
    font-family: monospace;
}
QPushButton#dialogPrimary:hover {
    border-color: #00ff88;
    background: #0a1a14;
}
QPushButton#dialogSecondary {
    background: #0a0a1a;
    border: 1px solid #1a1a4e;
    border-radius: 5px;
    color: #ccc;
    padding: 9px 0;
    font-size: 13px;
    font-family: monospace;
}
QPushButton#dialogSecondary:hover {
    border-color: #ff6688;
    color: #ff6688;
}
```

### 3. SettingsWidget 脏检测

新增方法 + 信号：

```cpp
// SettingsWidget.h
signals:
    void backClicked();
    void settingsChanged();  // 新增：设置变更且游戏中

public:
    /// \brief 记录当前游戏使用的设置值（用于脏检测）
    void setGameSettings(int speedMs, int boardSize);

private:
    int m_gameSpeedMs = -1;   // -1 表示无活跃游戏
    int m_gameBoardSize = -1;
```

backClicked 按钮点击处理改为：

```cpp
// 构造中修改 back 按钮 connect：
connect(btnBack, &QPushButton::clicked, this, [this]() {
    if (m_gameSpeedMs > 0 && m_gameBoardSize > 0) {
        // 有活跃游戏——检测设置是否变化
        if (speedMs() != m_gameSpeedMs || boardSize() != m_gameBoardSize) {
            emit settingsChanged();
            return;
        }
    }
    emit backClicked();
});
```

### 4. AppShell 接线

**startSinglePlayer** 末尾追加：
```cpp
m_settingsWidget->setGameSettings(speedMs, boardSize);
```

**构造中新增 connect**：
```cpp
connect(m_settingsWidget, &SettingsWidget::settingsChanged, this, [this]() {
    auto* dlg = new SettingsDialog(this);
    connect(dlg, &SettingsDialog::restartClicked, this, [this, dlg]() {
        m_settingsWidget->hide();
        startSinglePlayer();
    });
    connect(dlg, &SettingsDialog::laterClicked, this, [this, dlg]() {
        m_settingsWidget->hide();
    });
    dlg->show();
});
```

**showMenu** 中清除游戏设置标记：
```cpp
m_settingsWidget->setGameSettings(-1, -1);
```

### 5. CMakeLists.txt

```
src/app/CMakeLists.txt 追加：
  SettingsDialog.cpp
```

---

## 验收标准

1. 编译 /W4 零警告
2. 45 测试全绿
3. 单人模式 Playing 中 → ESC 暂停 → 设置 → 修改速度/大小 → 点返回 → 弹窗出现
4. 弹窗 TRON 暗色风格：绿色边框、暗底、"!" 图标、"设 置 已 更 改" + "将在下一局生效"
5. 点"重新开始"：关闭弹窗+设置 → 新游戏以新参数开始
6. 点"稍后再说"：关闭弹窗+设置 → 继续旧游戏，下局生效
7. 未修改任何设置时点返回 → 直接关闭，不弹窗
8. 主菜单状态下进设置 → 直接返回，不弹窗（无活跃游戏）
9. `src/app/` 中零 `setStyleSheet`

---

## 提交格式

```
改动文件清单:
+ src/app/SettingsDialog.h
+ src/app/SettingsDialog.cpp
~ src/app/SettingsWidget.h
~ src/app/SettingsWidget.cpp
~ src/app/AppShell.cpp
~ src/app/CMakeLists.txt
~ src/resources/style/main.qss

新增/修改测试列表:
无测试变更，45 全绿。

一句冒烟:
编译 /W4 零警告，45 测试全绿，SnakeArena.exe：
游戏中改设置→返回→弹窗 TRON 风格 + 重新开始生效 + 稍后下局生效，
未修改不弹窗，主菜单不弹窗，src/app/ 零 setStyleSheet。
```
