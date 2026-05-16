#pragma once
#include <QWidget>

/// \brief 暂停浮层：继续/重新开始/设置/返回主菜单
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
