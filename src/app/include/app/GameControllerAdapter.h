#ifndef APP_GAMECONTROLLERADAPTER_H
#define APP_GAMECONTROLLERADAPTER_H

#include <QObject>
#include <QTimer>
#include <memory>

#include "core/engine/GameController.h"
#include "core/engine/GameObserver.h"
#include "core/state/StateMachine.h"

class GameControllerAdapter : public QObject, public core::GameObserver {
    Q_OBJECT

public:
    explicit GameControllerAdapter(const core::GameConfig& config = core::GameConfig{},
                                   QObject* parent = nullptr);

    void onEvent(const core::GameEvent& event) override;

    core::GameController& controller() { return *controller_; }
    core::StateMachine& stateMachine() { return *stateMachine_; }
    const core::GameConfig& config() const { return controller_->config(); }

    void setConfig(const core::GameConfig& config);

    void startGame();
    void togglePause();
    void returnToMenu();
    void handleInput(core::Direction d);

    int score() const { return controller_->score(); }
    int tickInterval() const { return tickTimer_->interval(); }

    void stopTickTimer() { tickTimer_->stop(); }

signals:
    void ticked();
    void directionInputted(core::Direction d);
    void foodEaten(int score, core::Point pos);
    void gameOver(int score);
    void stateChanged(core::StateType type);

private:
    std::unique_ptr<core::GameController> controller_;
    std::unique_ptr<core::StateMachine> stateMachine_;
    QTimer* tickTimer_;

private slots:
    void onTickTimer();
};

#endif // APP_GAMECONTROLLERADAPTER_H
