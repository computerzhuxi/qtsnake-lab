#ifndef SNAKE_APP_MAINMENUWIDGET_H
#define SNAKE_APP_MAINMENUWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>

/// \brief 主菜单页面：标题、四模式按钮、设置/退出入口
class MainMenuWidget : public QWidget {
    Q_OBJECT
public:
    explicit MainMenuWidget(QWidget* parent = nullptr);

    void enter() {}
    void exit() {}

signals:
    void singlePlayerClicked();
    void aiBattleClicked();
    void multiplayerClicked();
    void replayClicked();
    void settingsClicked();
    void exitClicked();
};

#endif // SNAKE_APP_MAINMENUWIDGET_H
