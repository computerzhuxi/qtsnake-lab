# T-21：ShineLabel 反光倒计时文字

> 签发：主程 | 日期：2026-05-21 | 状态：待领取
> 分支：`phase-3-collision-refactor`
> 前置卡：T-20（PauseWidget / GameOverWidget 面板化已通过）

---

## 任务描述

实现 `ShineLabel`——自定义 QLabel 子类，用 QPainter + QLinearGradient 绘制从左向右扫过的反光文字动画。替换 GamePage 中"按任意键开始"提示文字（当前使用普通 QLabel + QSS property selector）。

---

## 输入约束

### 允许修改/新增的文件

```
+ src/app/ShineLabel.h              (新增)
+ src/app/ShineLabel.cpp            (新增)
~ src/app/GamePage.h                (新增 m_shineLabel 成员)
~ src/app/GamePage.cpp              (showCountdown 改用 ShineLabel)
~ src/app/CMakeLists.txt            (追加 ShineLabel.cpp)
~ src/resources/style/main.qss      (删除 [state="hint"] 规则，ShineLabel 不依赖 QSS 动画)
```

### 不允许做的事

- 不得修改 core / controller / ui 层任何文件
- 不得在 `src/app/` 中使用 `setStyleSheet`
- 不得修改 GamePage 布局骨架
- 不得使用 `using namespace std;`
- 不得使用 Qt CSS `animation`（Qt5 不支持）

---

## 输出约束

### 1. ShineLabel 类

```cpp
class ShineLabel : public QLabel {
    Q_OBJECT
public:
    explicit ShineLabel(QWidget* parent = nullptr);

    void startShine();   // 启动反光动画（循环）
    void stopShine();     // 停止动画，恢复静止暗色文字

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QTimer* m_timer;
    qreal m_phase;    // 0.0 → 1.0，控制亮带水平位置
    bool m_active;
};
```

### 2. 绘制逻辑（paintEvent）

**重要**：使用 `contentsRect()` 而非 `rect()`，这样 GamePage 设置的 `contentsMargins` 可以控制文字绘制区域。

静止状态（`m_active == false`）：直接用 `QPen(#556)` 绘制文字。

动画状态（`m_active == true`）：构建 3 倍 widget 宽度的 QLinearGradient，亮带位置随 `m_phase` 偏移。

**颜色**（来自 HTML `.text-shine`）：

| 位置 | 颜色 | 说明 |
|---|---|---|
| 0.00 | `#556` | 暗 |
| 0.40 | `#556` | 暗 |
| 0.44 | `#1a3a2a` | 半亮（#00ff88 低透明度在暗底上的近似） |
| 0.47 | `#00ff88` | 亮带左缘 |
| 0.50 | `#00ff88` | 亮带中心 |
| 0.53 | `#1a3a2a` | 半亮 |
| 0.56 | `#556` | 暗 |
| 1.00 | `#556` | 暗 |

**动画偏移**：
```cpp
qreal w = contentsRect().width();
qreal span = w * 3.0;
qreal offset = m_phase * span - w;
QLinearGradient gradient(contentsRect().left() + offset, 0,
                         contentsRect().left() + offset + span, 0);
// 设置色标...
painter.setPen(QPen(QBrush(gradient), 0));
painter.setFont(font());
painter.drawText(contentsRect(), alignment(), text());
```

phase 0→1 时亮带从文字左外侧扫入、右外侧扫出，循环即产生连续扫光。周期 4 秒，与 HTML `animation: shine 4s` 一致。

### 3. 动画驱动

```cpp
// 30fps ≈ 33ms interval
// 4 秒周期 → 120 ticks → 每 tick 增 1/120

startShine():
    m_active = true;
    m_phase = 0.0;
    m_timer->start(33);

stopShine():
    m_active = false;
    m_timer->stop();
    update();   // 重绘为静止暗色
```

每 tick：`m_phase += 1.0 / 120.0;` 若 `>= 1.0` 归零；调 `update()`。

### 4. 文字位置（关键）

HTML 原型中 hint 文字距底部 60px：`.countdown-hint-wrap { position: absolute; bottom: 60px; }`。倒计时数字（3/2/1/GO!）则是完全居中。

**GamePage 构造中设置**：
```cpp
m_shineLabel = new ShineLabel(this);
m_shineLabel->setObjectName("shineLabel");
m_shineLabel->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
m_shineLabel->setContentsMargins(0, 0, 0, 60);   // 底部留白 60px
m_shineLabel->setFont(QFont("Consolas", 18));
m_shineLabel->hide();
```

`setContentsMargins(0, 0, 0, 60)` 配合 `contentsRect()` 绘制 + `AlignBottom` = 文字绘制在距底部 60px 处，与 HTML 一致。

### 5. GamePage 集成

**showCountdown** 修改：
```cpp
void GamePage::showCountdown(int number) {
    if (number == -1) {
        // hint — ShineLabel
        m_countdownLabel->hide();
        m_shineLabel->setText(tr("按任意键开始"));
        m_shineLabel->show();
        m_shineLabel->raise();
        m_shineLabel->startShine();
        return;
    }
    // countdown numbers — 原 QLabel 逻辑完全保留
    m_shineLabel->stopShine();
    m_shineLabel->hide();

    QString text;
    Qt::Alignment align;
    if (number == 0) {
        text = "GO!";
        m_countdownLabel->setProperty("state", "go");
        align = Qt::AlignCenter;
    } else {
        text = QString::number(number);
        m_countdownLabel->setProperty("state", "count");
        align = Qt::AlignCenter;
    }
    m_countdownLabel->style()->unpolish(m_countdownLabel);
    m_countdownLabel->style()->polish(m_countdownLabel);
    m_countdownLabel->setAlignment(align);
    m_countdownLabel->setText(text);
    m_countdownLabel->show();
    m_countdownLabel->raise();
}
```

**resizeEvent** 追加：
```cpp
m_shineLabel->setGeometry(0, 0, w, h);
```

**hideAllOverlays** 追加：
```cpp
m_shineLabel->stopShine();
m_shineLabel->hide();
```

### 6. main.qss 清理

删除 `QLabel#CountdownLabel[state="hint"]` 规则——hint 状态改由 ShineLabel 纯代码绘制，不再走 QSS。

保留 `[state="count"]` 和 `[state="go"]`——倒计时数字仍用原 QLabel + QSS。

### 7. CMakeLists.txt

```
src/app/CMakeLists.txt snake_app STATIC 源列表追加：
  ShineLabel.cpp
```

---

## 验收标准

1. 编译 /W4 零警告
2. 45 测试全绿
3. 主菜单 → 单人模式 → "按任意键开始"绿色亮带从左向右匀速扫过（~4秒/周期）
4. 文字位置距底部 ~60px，与 HTML 原型 `.countdown-hint-wrap { bottom: 60px }` 一致
5. 按下任意键后亮带消失，倒计时数字 3→2→1→GO! 完全居中正常显示
6. `src/app/` 中零 `setStyleSheet`
7. 动画纯 QTimer + QPainter，无 CSS animation 依赖

---

## 提交格式

```
改动文件清单:
+ src/app/ShineLabel.h
+ src/app/ShineLabel.cpp
~ src/app/GamePage.h
~ src/app/GamePage.cpp
~ src/app/CMakeLists.txt
~ src/resources/style/main.qss    (删除 [state="hint"] 规则)

新增/修改测试列表:
无测试变更，45 全绿。

一句冒烟:
编译 /W4 零警告，45 测试全绿，SnakeArena.exe：
单人模式 → "按任意键开始"绿色亮带从左向右扫描（4秒周期），
文字距底部 ~60px，按下后倒计时 3→2→1→GO! 居中正常，
src/app/ 零 setStyleSheet。
```
