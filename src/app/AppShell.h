#ifndef SNAKE_APP_APPSHELL_H
#define SNAKE_APP_APPSHELL_H

#include <QWidget>
#include <QStackedWidget>
#include "GameController.h"
#include "GameView.h"
#include "GameScene.h"

class MainMenuWidget;
class GamePage;
class SettingsWidget;

/// \brief 应用外壳：QStackedWidget 管理菜单页/游戏页，Settings 全局浮层
class AppShell : public QWidget {
    Q_OBJECT
public:
    explicit AppShell(QWidget* parent = nullptr);
    ~AppShell();

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onControllerStateChanged(GameController::State state);
    void onCountdownTick(int number);

private:
    void setupUI();
    void showMenu();
    void showGame();
    void showSettings();
    void startSinglePlayer();

    QStackedWidget* m_stack;
    MainMenuWidget* m_mainMenu;
    GamePage* m_gamePage;
    SettingsWidget* m_settingsWidget;
    GameController* m_controller;
    int m_currentScore = 0;
};

#endif // SNAKE_APP_APPSHELL_H
