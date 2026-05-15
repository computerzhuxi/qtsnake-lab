#include "core/engine/Direction.h"
#include "core/engine/Point.h"
#include "core/engine/Grid.h"
#include "core/engine/Snake.h"
#include "core/engine/Food.h"
#include "core/engine/GameConfig.h"
#include "core/engine/GameController.h"
#include "core/engine/GameEvent.h"

#include <iostream>
#include <cassert>

using namespace core;

static int testsPassed = 0;
static int testsFailed = 0;

void check(bool condition, const char* testName) {
    if (condition) {
        ++testsPassed;
        std::cout << "  PASS: " << testName << std::endl;
    } else {
        ++testsFailed;
        std::cerr << "  FAIL: " << testName << std::endl;
    }
}

int main() {
    std::cout << "=== QtSnake Lab Core Engine Tests ===" << std::endl;

    // Direction
    std::cout << "[Direction]" << std::endl;
    check(opposite(Direction::Up) == Direction::Down, "opposite(Up) == Down");
    check(opposite(Direction::Down) == Direction::Up, "opposite(Down) == Up");
    check(opposite(Direction::Left) == Direction::Right, "opposite(Left) == Right");
    check(opposite(Direction::Right) == Direction::Left, "opposite(Right) == Left");

    // Point
    std::cout << "[Point]" << std::endl;
    check(Point{1, 2} == Point{1, 2}, "equality");
    check(Point{1, 2} != Point{2, 1}, "inequality");
    check((Point{1, 2} + Point{3, 4}) == Point{4, 6}, "addition");

    // Grid
    std::cout << "[Grid]" << std::endl;
    Grid grid(10, 8);
    check(grid.isInside({0, 0}), "corner inside");
    check(grid.isInside({9, 7}), "max corner inside");
    check(!grid.isInside({-1, 0}), "left out of bounds");
    check(!grid.isInside({10, 0}), "right out of bounds");
    check(!grid.isInside({0, -1}), "top out of bounds");
    check(!grid.isInside({0, 8}), "bottom out of bounds");

    Point p{5, 3};
    check(!grid.isOccupied(p), "not occupied initially");
    grid.occupy(p);
    check(grid.isOccupied(p), "occupied after occupy");
    grid.clear(p);
    check(!grid.isOccupied(p), "not occupied after clear");

    // Snake
    std::cout << "[Snake]" << std::endl;
    Snake snake({5, 5}, 3, Direction::Right);
    check(snake.length() == 3, "initial length");
    check(snake.head() == Point{5, 5}, "initial head position");
    check(snake.direction() == Direction::Right, "initial direction");
    check(snake.alive(), "alive initially");

    snake.moveOneStep();
    check(snake.head() == Point{6, 5}, "move right");
    check(snake.length() == 3, "length unchanged after move");

    snake.setDirection(Direction::Left); // should be rejected
    check(snake.direction() == Direction::Right, "reverse direction rejected");

    snake.setDirection(Direction::Up);
    check(snake.direction() == Direction::Up, "valid direction change");

    // Growth — grow(2) means 2 more segments over next 2 moves
    snake.grow(2);
    snake.moveOneStep();
    check(snake.length() == 4, "length 4 after 1st grow step");
    snake.moveOneStep();
    check(snake.length() == 5, "length 5 after 2nd grow step");
    snake.moveOneStep();
    check(snake.length() == 5, "length stays 5 after growth consumed");

    // Food
    std::cout << "[Food]" << std::endl;
    Grid foodGrid(10, 10);
    Snake foodSnake({5, 5}, 3, Direction::Right);
    Food food;
    food.respawn(foodGrid, foodSnake);
    check(foodGrid.isInside(food.position()), "food inside grid");
    check(food.value() >= 1 && food.value() <= 3, "food value in range");

    // GameController
    std::cout << "[GameController]" << std::endl;
    GameController controller;
    check(controller.score() == 0, "initial score 0");
    check(!controller.isGameOver(), "not game over initially");
    check(controller.tickNumber() == 0, "tick number starts at 0");

    controller.start();
    controller.tick();
    check(controller.tickNumber() == 1, "tick advances");

    // Wall collision
    GameConfig smallConfig;
    smallConfig.gridWidth = 5;
    smallConfig.gridHeight = 5;
    GameController smallController(smallConfig);
    smallController.start();
    // snake starts at (2,2) heading right, hits wall at x=3 (4,4 is edge)
    for (int i = 0; i < 5; ++i) {
        smallController.tick();
    }
    check(smallController.isGameOver(), "wall collision triggers game over");

    // Pause
    GameController ctrl2;
    ctrl2.start();
    ctrl2.tick();
    ctrl2.pause();
    uint64_t pausedTick = ctrl2.tickNumber();
    ctrl2.tick();
    check(ctrl2.tickNumber() == pausedTick, "pause stops tick");

    ctrl2.resume();
    ctrl2.tick();
    check(ctrl2.tickNumber() == pausedTick + 1, "resume allows tick");

    // Observer
    struct TestObserver : GameObserver {
        int tickCount = 0;
        void onEvent(const GameEvent& e) override {
            if (e.type == GameEventType::Tick) ++tickCount;
        }
    };

    GameController ctrl3;
    ctrl3.start();
    TestObserver obs;
    ctrl3.addObserver(&obs);
    ctrl3.tick();
    ctrl3.tick();
    ctrl3.tick();
    check(obs.tickCount == 3, "observer receives 3 ticks");

    ctrl3.removeObserver(&obs);
    ctrl3.tick();
    check(obs.tickCount == 3, "observer removed");

    // Reset
    ctrl3.reset();
    check(ctrl3.score() == 0, "reset clears score");
    check(ctrl3.tickNumber() == 0, "reset clears ticks");
    check(!ctrl3.isGameOver(), "reset clears game over");

    // Results
    std::cout << std::endl;
    std::cout << "=== Results: " << testsPassed << " passed, "
              << testsFailed << " failed ===" << std::endl;

    return testsFailed > 0 ? 1 : 0;
}
