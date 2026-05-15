#include "app/GameControllerAdapter.h"

#include "core/engine/GameEvent.h"
#include "core/logging/Logger.h"

GameControllerAdapter::GameControllerAdapter(const core::GameConfig& config,
                                           QObject* parent)
    : QObject(parent)
    , controller_(std::make_unique<core::GameController>(config))
    , stateMachine_(std::make_unique<core::StateMachine>(*controller_))
    , tickTimer_(new QTimer(this))
{
    controller_->addObserver(this);
    tickTimer_->setInterval(controller_->config().initialSpeed);
    connect(tickTimer_, &QTimer::timeout, this, &GameControllerAdapter::onTickTimer);
}

void GameControllerAdapter::setConfig(const core::GameConfig& config) {
    controller_->removeObserver(this);
    controller_ = std::make_unique<core::GameController>(config);
    controller_->addObserver(this);
    stateMachine_ = std::make_unique<core::StateMachine>(*controller_);
    tickTimer_->setInterval(config.initialSpeed);
}

void GameControllerAdapter::onEvent(const core::GameEvent& event) {
    switch (event.type) {
    case core::GameEventType::Tick:
        LOG_INFO("Adapter: emitting ticked, tickNum="
                 + std::to_string(event.tickNumber));
        emit ticked();
        break;
    case core::GameEventType::Input:
        emit directionInputted(event.inputDirection);
        break;
    case core::GameEventType::FoodEaten:
        emit foodEaten(event.score, event.foodPosition);
        break;
    case core::GameEventType::GameOver:
        emit gameOver(event.score);
        break;
    default:
        break;
    }
}

void GameControllerAdapter::startGame() {
    stateMachine_->startGame();
    tickTimer_->start();
    emit stateChanged(stateMachine_->currentType());
}

void GameControllerAdapter::togglePause() {
    stateMachine_->togglePause();
    if (stateMachine_->currentType() == core::StateType::Paused) {
        tickTimer_->stop();
    } else if (stateMachine_->currentType() == core::StateType::Playing) {
        tickTimer_->start();
    }
    emit stateChanged(stateMachine_->currentType());
}

void GameControllerAdapter::returnToMenu() {
    tickTimer_->stop();
    stateMachine_->returnToMenu();
    emit stateChanged(stateMachine_->currentType());
}

void GameControllerAdapter::handleInput(core::Direction d) {
    stateMachine_->handleInput(d);
}

void GameControllerAdapter::onTickTimer() {
    auto beforeType = stateMachine_->currentType();
    stateMachine_->update();
    auto afterType = stateMachine_->currentType();

    if (afterType != beforeType) {
        emit stateChanged(afterType);
    }

    if (afterType == core::StateType::GameOver) {
        tickTimer_->stop();
    }
}
