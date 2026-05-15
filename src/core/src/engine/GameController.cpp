#include "core/engine/GameController.h"
#include <algorithm>
#include <random>

namespace core {

GameController::GameController(const GameConfig& config)
    : config_(config)
    , grid_(config.gridWidth, config.gridHeight)
    , snake_(Point{config.gridWidth / 2, config.gridHeight / 2},
             config.initialSnakeLength, Direction::Right)
    , nextDirection_(Direction::Right)
{
    std::random_device rd;
    rngSeed_ = rd();
    food_.setSeed(rngSeed_);
    food_.respawn(grid_, snake_);
}

void GameController::tick() {
    if (gameOver_ || paused_) return;

    ++tickNumber_;

    if (directionQueued_) {
        snake_.setDirection(nextDirection_);
        directionQueued_ = false;
    }

    snake_.moveOneStep();

    checkCollisions();
    if (!gameOver_) {
        checkFood();
    }

    GameEvent tickEvent;
    tickEvent.type = GameEventType::Tick;
    tickEvent.tickNumber = tickNumber_;
    tickEvent.score = score_;
    notifyObservers(tickEvent);
}

void GameController::setDirection(Direction d) {
    nextDirection_ = d;
    directionQueued_ = true;

    GameEvent inputEvent;
    inputEvent.type = GameEventType::Input;
    inputEvent.inputDirection = d;
    inputEvent.tickNumber = tickNumber_;
    notifyObservers(inputEvent);
}

void GameController::start() {
    gameOver_ = false;
    paused_ = false;
}

void GameController::pause() {
    paused_ = true;
}

void GameController::resume() {
    paused_ = false;
}

void GameController::reset() {
    std::random_device rd;
    rngSeed_ = rd();
    grid_ = Grid(config_.gridWidth, config_.gridHeight);
    snake_ = Snake(Point{config_.gridWidth / 2, config_.gridHeight / 2},
                   config_.initialSnakeLength, Direction::Right);
    nextDirection_ = Direction::Right;
    directionQueued_ = false;
    score_ = 0;
    tickNumber_ = 0;
    gameOver_ = false;
    paused_ = false;
    food_.setSeed(rngSeed_);
    food_.respawn(grid_, snake_);
}

void GameController::checkCollisions() {
    Point h = snake_.head();

    // Wall collision
    if (!grid_.isInside(h)) {
        snake_.kill();
        gameOver_ = true;

        GameEvent event;
        event.type = GameEventType::Collision;
        event.score = score_;
        event.tickNumber = tickNumber_;
        notifyObservers(event);

        GameEvent goEvent;
        goEvent.type = GameEventType::GameOver;
        goEvent.score = score_;
        goEvent.tickNumber = tickNumber_;
        notifyObservers(goEvent);
        return;
    }

    // Self collision — check against all body segments except the head
    const auto& body = snake_.body();
    for (size_t i = 1; i < body.size(); ++i) {
        if (body[i] == h) {
            snake_.kill();
            gameOver_ = true;

            GameEvent event;
            event.type = GameEventType::Collision;
            event.score = score_;
            event.tickNumber = tickNumber_;
            notifyObservers(event);

            GameEvent goEvent;
            goEvent.type = GameEventType::GameOver;
            goEvent.score = score_;
            goEvent.tickNumber = tickNumber_;
            notifyObservers(goEvent);
            return;
        }
    }
}

void GameController::checkFood() {
    if (snake_.head() == food_.position()) {
        score_ += food_.value();
        snake_.grow(food_.value());

        GameEvent event;
        event.type = GameEventType::FoodEaten;
        event.foodPosition = food_.position();
        event.score = score_;
        event.tickNumber = tickNumber_;
        notifyObservers(event);

        food_.respawn(grid_, snake_);

        GameEvent spawnEvent;
        spawnEvent.type = GameEventType::FoodSpawned;
        spawnEvent.foodPosition = food_.position();
        spawnEvent.score = score_;
        spawnEvent.tickNumber = tickNumber_;
        notifyObservers(spawnEvent);
    }
}

void GameController::addObserver(GameObserver* observer) {
    observers_.push_back(observer);
}

void GameController::removeObserver(GameObserver* observer) {
    observers_.erase(
        std::remove(observers_.begin(), observers_.end(), observer),
        observers_.end());
}

void GameController::notifyObservers(const GameEvent& event) {
    for (auto* obs : observers_) {
        obs->onEvent(event);
    }
}

void GameController::setRngSeed(unsigned int seed) {
    rngSeed_ = seed;
    food_.setSeed(seed);
}

void GameController::setInitialState(
    const std::deque<Point>& snakeBody,
    Point foodPos, Direction dir, int score)
{
    snake_.setBody(snakeBody, dir);
    grid_.clearAll();
    for (const auto& seg : snakeBody) {
        grid_.occupy(seg);
    }
    food_.setPosition(foodPos);
    nextDirection_ = dir;
    directionQueued_ = false;
    score_ = score;
    tickNumber_ = 0;
    gameOver_ = false;
    paused_ = false;
}

} // namespace core
