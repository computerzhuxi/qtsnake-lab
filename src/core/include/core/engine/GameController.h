#ifndef CORE_ENGINE_GAMECONTROLLER_H
#define CORE_ENGINE_GAMECONTROLLER_H

#include "Direction.h"
#include "GameConfig.h"
#include "GameEvent.h"
#include "GameObserver.h"
#include "Grid.h"
#include "Snake.h"
#include "Food.h"
#include <vector>

namespace core {

class GameController {
public:
    explicit GameController(const GameConfig& config = GameConfig{});

    void tick();
    void setDirection(Direction d);

    void start();
    void pause();
    void resume();
    void reset();

    Snake& snake() { return snake_; }
    const Snake& snake() const { return snake_; }
    Grid& grid() { return grid_; }
    const Grid& grid() const { return grid_; }
    Food& food() { return food_; }
    const Food& food() const { return food_; }

    int score() const { return score_; }
    const GameConfig& config() const { return config_; }
    bool isGameOver() const { return gameOver_; }
    bool isPaused() const { return paused_; }
    uint64_t tickNumber() const { return tickNumber_; }

    void addObserver(GameObserver* observer);
    void removeObserver(GameObserver* observer);

    void setRngSeed(unsigned int seed);
    unsigned int rngSeed() const { return rngSeed_; }
    void setInitialState(const std::deque<Point>& snakeBody,
                         Point foodPos, Direction dir, int score);

private:
    GameConfig config_;
    Grid grid_;
    Snake snake_;
    Food food_;
    unsigned int rngSeed_ = 0;
    int score_ = 0;
    uint64_t tickNumber_ = 0;
    bool gameOver_ = false;
    bool paused_ = false;

    Direction nextDirection_;
    bool directionQueued_ = false;

    std::vector<GameObserver*> observers_;

    void checkCollisions();
    void checkFood();
    void notifyObservers(const GameEvent& event);

    Point randomFreeCell() const;
};

} // namespace core

#endif // CORE_ENGINE_GAMECONTROLLER_H
