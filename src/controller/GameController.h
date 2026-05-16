#ifndef SNAKE_CONTROLLER_GAMECONTROLLER_H
#define SNAKE_CONTROLLER_GAMECONTROLLER_H

#include <QObject>
#include <QTimer>
#include "Board.h"
#include "Snake.h"
#include "Direction.h"

class GameScene;

/// \brief 游戏控制器：QTimer 驱动主循环，管理状态机和输入分发
class GameController : public QObject {
    Q_OBJECT
public:
    enum class State { Idle, Ready, Countdown, Playing, Paused, GameOver };
    explicit GameController(GameScene* scene, QObject* parent = nullptr);
    void startGame(int boardW = 20, int boardH = 20, int speedMs = 100);
    void pause();
    void resume();
    void reset();
    void handleKeyPress(Direction dir);
    void handleReadyKey();
    State state() const;
    Board* board();
    Snake* playerSnake();
    int score() const;

signals:
    void stateChanged(State newState);
    void countdownTick(int number);
    void scoreChanged(int score);

private slots:
    void tick();

private:
    GameScene* m_scene;
    QTimer* m_timer;
    Board* m_board = nullptr;
    Snake* m_playerSnake = nullptr;
    State m_state = State::Idle;
    int m_score = 0;
    int m_countdownValue = 3;
};

#endif // SNAKE_CONTROLLER_GAMECONTROLLER_H
