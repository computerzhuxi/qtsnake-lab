#ifndef SNAKE_APP_MAINMENUWIDGET_H
#define SNAKE_APP_MAINMENUWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>

/// \brief 主菜单页面
/// \details 标题 + 蛇形装饰 + 四模式按钮（单机可用，其余占位）+ 设置/退出链接。
///          使用 enter/exit 生命周期（当前无需额外操作）。

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
