#ifndef SNAKE_APP_PAUSEWIDGET_H
#define SNAKE_APP_PAUSEWIDGET_H

#include <QWidget>

/// \brief 暂停浮层
/// \details 半透明遮罩 + 居中弹窗。四个按钮：继续/重新开始/设置/返回主菜单。

class PauseWidget : public QWidget {
    Q_OBJECT
public:
    explicit PauseWidget(QWidget* parent = nullptr);

signals:
    void resumeClicked();
    void restartClicked();
    void settingsClicked();
    void menuClicked();
};

#endif // SNAKE_APP_PAUSEWIDGET_H
