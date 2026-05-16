#ifndef SNAKE_APP_APPSHELL_H
#define SNAKE_APP_APPSHELL_H

#include <QWidget>
#include "GameController.h"
#include "GameView.h"
#include "GameScene.h"

class MainMenuWidget;
class PauseWidget;
class GameOverWidget;
class SettingsWidget;

/// \brief 应用外壳：管理游戏底层与浮层叠加，分发键盘事件，协调各浮层切换
class AppShell : public QWidget {
    Q_OBJECT
public:
    explicit AppShell(QWidget* parent = nullptr);
    ~AppShell();
    void showMainMenu();
    void showPause();
    void showGameOver();
    void showSettings();
    void hideAllOverlays();
    void startSinglePlayer();

protected:
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onControllerStateChanged(GameController::State state);

private:
    void setupUI();
    GameScene* m_scene;
    GameView* m_view;
    GameController* m_controller;
    MainMenuWidget* m_mainMenu;
    PauseWidget* m_pauseWidget;
    GameOverWidget* m_gameOver;
    SettingsWidget* m_settingsWidget;
};

#endif // SNAKE_APP_APPSHELL_H
