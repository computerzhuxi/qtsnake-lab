#ifndef SNAKE_APP_GAMEPAGE_H
#define SNAKE_APP_GAMEPAGE_H

#include <QWidget>
#include "GameController.h"
#include "GameView.h"
#include "GameScene.h"

class PauseWidget;
class GameOverWidget;
class QLabel;

/// \brief 游戏页面
/// \details GameView 底层 + 三个浮层（暂停/结算/倒计时）。
///          enter() 清空场景，exit() 清空场景并隐藏所有浮层。

class GamePage : public QWidget {
    Q_OBJECT
public:
    explicit GamePage(QWidget* parent = nullptr);

    GameScene* scene() const;
    GameView* view() const;

    void enter();
    void exit();

    void showPause();
    void showGameOver(int score, int length, int kills, int seconds);
    void showCountdown(int number);
    void hideAllOverlays();

signals:
    void resumeClicked();
    void restartClicked();
    void menuClicked();
    void settingsClicked();
    void playAgainClicked();

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    GameScene* m_scene;                     ///< 渲染场景
    GameView* m_view;                       ///< 渲染视口
    PauseWidget* m_pauseWidget;             ///< 暂停浮层
    GameOverWidget* m_gameOver;             ///< 结算浮层
    QLabel* m_countdownLabel;               ///< 倒计时/提示文字
};

#endif // SNAKE_APP_GAMEPAGE_H
