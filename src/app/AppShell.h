#ifndef SNAKE_APP_APPSHELL_H
#define SNAKE_APP_APPSHELL_H

#include <QWidget>
#include <QStackedWidget>
#include <memory>
#include "GameController.h"
#include "GameView.h"
#include "GameScene.h"

class MainMenuWidget;
class GamePage;
class SettingsWidget;
/// \brief 应用外壳（顶层窗口）
/// \details 使用 QStackedWidget 管理两个页面：
///          - Page 0: 主菜单
///          - Page 1: 游戏画面 + 浮层
///          SettingsWidget 为全局浮层，覆盖在 QStackedWidget 上方。

class AppShell : public QWidget {
    Q_OBJECT
public:
    explicit AppShell(QWidget* parent = nullptr);

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

    QStackedWidget* m_stack;                ///< 页面容器（菜单 / 游戏）
    MainMenuWidget* m_mainMenu;             ///< 主菜单页面
    GamePage* m_gamePage;                   ///< 游戏页面
    SettingsWidget* m_settingsWidget;       ///< 全局设置浮层
    std::unique_ptr<GameController> m_controller; ///< 当前游戏控制器（无游戏时为 nullptr）
    int m_currentScore = 0;                       ///< 当前分数
};

#endif // SNAKE_APP_APPSHELL_H
