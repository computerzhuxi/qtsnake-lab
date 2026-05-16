#pragma once
#include <QWidget>
#include <QPushButton>
#include <QLabel>

/// \brief 主菜单浮层：标题、四模式按钮、设置/退出入口
class MainMenuWidget : public QWidget {
    Q_OBJECT
public:
    explicit MainMenuWidget(QWidget* parent = nullptr);

signals:
    void singlePlayerClicked();
    void aiBattleClicked();
    void multiplayerClicked();
    void replayClicked();
    void settingsClicked();
    void exitClicked();
};
