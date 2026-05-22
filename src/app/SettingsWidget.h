#ifndef SNAKE_APP_SETTINGSWIDGET_H
#define SNAKE_APP_SETTINGSWIDGET_H

#include <QWidget>

class QStackedWidget;
class QPushButton;
class QComboBox;

/// \brief 设置全局浮层
/// \details 半透明遮罩 + 居中面板（与 T-20 同模式）。
///          自定义 Tab 按钮 + QStackedWidget：
///          - 操作：键位选择（方向键/WASD）、语言切换（中文/English）
///          - 游戏：速度（慢/中/快）、大小（小/中/大）、音量滑块

class SettingsWidget : public QWidget {
    Q_OBJECT
public:
    explicit SettingsWidget(QWidget* parent = nullptr);

    /// \brief 当前选中的 tick 间隔（慢=150, 中=100, 快=60）
    int speedMs() const;

    /// \brief 当前选中的棋盘尺寸（小=15, 中=20, 大=30）
    int boardSize() const;

    /// \brief 当前选中的键位方案（"arrows" / "wasd"）
    QString keyBinding() const;

    /// \brief 记录当前游戏使用的设置值（用于脏检测，-1 表示无活跃游戏）
    void setGameSettings(int speedMs, int boardSize);

signals:
    void backClicked();
    void settingsChanged();

private:
    void refreshStyle(QWidget* w);
    void loadSettings();
    void saveSettings() const;

    QStackedWidget* m_stack;
    QPushButton* m_btnControl;
    QPushButton* m_btnGame;
    QComboBox* m_speedCombo;
    QComboBox* m_sizeCombo;
    QComboBox* m_keyCombo;
    int m_gameSpeedMs = -1;
    int m_gameBoardSize = -1;
};

#endif // SNAKE_APP_SETTINGSWIDGET_H
