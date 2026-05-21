#ifndef SNAKE_APP_SETTINGSWIDGET_H
#define SNAKE_APP_SETTINGSWIDGET_H

#include <QWidget>

class QStackedWidget;
class QPushButton;

/// \brief 设置全局浮层
/// \details 半透明遮罩 + 居中面板（与 T-20 同模式）。
///          自定义 Tab 按钮 + QStackedWidget：
///          - 操作：键位选择（方向键/WASD）、语言切换（中文/English）
///          - 游戏：速度（慢/中/快）、大小（小/中/大）、音量滑块

class SettingsWidget : public QWidget {
    Q_OBJECT
public:
    explicit SettingsWidget(QWidget* parent = nullptr);

signals:
    void backClicked();

private:
    void refreshStyle(QWidget* w);

    QStackedWidget* m_stack;
    QPushButton* m_btnControl;
    QPushButton* m_btnGame;
};

#endif // SNAKE_APP_SETTINGSWIDGET_H
