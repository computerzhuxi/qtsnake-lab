#ifndef SNAKE_APP_GAMEPAGE_H
#define SNAKE_APP_GAMEPAGE_H

#include <QWidget>
#include "GameController.h"
#include "GameView.h"
#include "GameScene.h"

class PauseWidget;
class GameOverWidget;
class QLabel;

/// \brief 游戏页面：GameView底层 + 暂停/结算/倒计时浮层
class GamePage : public QWidget {
    Q_OBJECT
public:
    explicit GamePage(QWidget* parent = nullptr);

    GameScene* scene() const;
    GameView* view() const;
    void setController(GameController* ctrl);

    void enter();
    void exit();

    void showPause();
    void showGameOver(int score, int length, int kills, int seconds);
    void showCountdown(int number);
    void hideAllOverlays();

    PauseWidget* pauseWidget() const;
    GameOverWidget* gameOverWidget() const;

signals:
    void resumeClicked();
    void restartClicked();
    void menuClicked();
    void settingsClicked();
    void playAgainClicked();

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    GameScene* m_scene;
    GameView* m_view;
    PauseWidget* m_pauseWidget;
    GameOverWidget* m_gameOver;
    QLabel* m_countdownLabel;
};

#endif // SNAKE_APP_GAMEPAGE_H
