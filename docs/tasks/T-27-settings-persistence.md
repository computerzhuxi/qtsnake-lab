# T-27：Settings 持久化（QSettings 落盘）

> 签发：主程 | 日期：2026-05-22 | 状态：待领取
> 分支：`phase-3-collision-refactor`
> 前置卡：T-26（Settings 弹窗已通过）

---

## 任务描述

SettingsWidget 的三项设置在 exe 关闭后恢复默认值（中/20×20/方向键）。加 QSettings 读写，实现跨 session 持久化。

---

## 输入约束

### 允许修改的文件

```
~ src/app/SettingsWidget.h     (新增 loadSettings/saveSettings 私有方法)
~ src/app/SettingsWidget.cpp   (构造中 load + ComboBox 切换时 save)
```

### 不允许做的事

- 不得修改其他任何文件
- 不得在 `src/app/` 中使用 `setStyleSheet`
- 不得使用 `using namespace std;`

---

## 输出约束

### QSettings key 定义

| 设置项 | key | 默认值 |
|---|---|---|
| 速度 | `game/speedMs` | 100（中） |
| 棋盘 | `game/boardSize` | 20（中） |
| 键位 | `control/keyBinding` | `arrows` |

### 实现

```cpp
// SettingsWidget.h 追加：
private:
    void loadSettings();
    void saveSettings() const;
```

**loadSettings**：构造末尾调用，读取 QSettings 覆盖 ComboBox 的 currentIndex：

```cpp
QSettings s;
int speedMs = s.value("game/speedMs", 100).toInt();
int sizes[] = {150, 100, 60};
for (int i = 0; i < 3; ++i) { if (sizes[i] == speedMs) { m_speedCombo->setCurrentIndex(i); break; } }

int boardSize = s.value("game/boardSize", 20).toInt();
int boards[] = {15, 20, 30};
for (int i = 0; i < 3; ++i) { if (boards[i] == boardSize) { m_sizeCombo->setCurrentIndex(i); break; } }

QString kb = s.value("control/keyBinding", "arrows").toString();
m_keyCombo->setCurrentIndex(kb == "wasd" ? 1 : 0);
```

**saveSettings**：在对应的 setter 中调用（或在 ComboBox currentIndexChanged 时）。最简单——在 load 之后给三个 ComboBox 连接信号：

```cpp
// 构造末尾（loadSettings 之后）：
for (auto* cb : {m_speedCombo, m_sizeCombo, m_keyCombo})
    connect(cb, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this]() { saveSettings(); });

void SettingsWidget::saveSettings() const {
    QSettings s;
    s.setValue("game/speedMs", speedMs());
    s.setValue("game/boardSize", boardSize());
    s.setValue("control/keyBinding", keyBinding());
}
```

利用已有的 `speedMs()` / `boardSize()` / `keyBinding()` getter，写入时直接取值。

### 初始化顺序

构造中面板布局搭建完成后 → loadSettings() → connect ComboBox signals → saveSettings（仅首次运行需写默认值，后续 load 后 currentIndex 变化会触发 save，但只在值真的变时才写）。

> 注意：`loadSettings` 中 setCurrentIndex 会触发 currentIndexChanged → saveSettings。这没问题——saveSettings 是幂等的，多写一次默认值无害。如果介意，可以在 load 前设置一个 `m_loading` flag 跳过 save。

---

## 验收标准

1. 编译 /W4 零警告
2. 45 测试全绿
3. 改速度为"快(60ms)"→关掉 exe→重新打开→进设置→速度显示"快(60ms)"
4. 改键位为 WASD→重启 exe→仍为 WASD
5. 改棋盘为"大(30×30)"→重启 exe→仍为大
6. 首次启动（无 QSettings 文件）默认值正确（中/20×20/方向键）

---

## 提交格式

```
改动文件清单:
~ src/app/SettingsWidget.h
~ src/app/SettingsWidget.cpp

新增/修改测试列表:
无测试变更，45 全绿。

一句冒烟:
编译 /W4 零警告，45 测试全绿，改速度→重启 exe→速度保持，改键位→重启→键位保持。
```
