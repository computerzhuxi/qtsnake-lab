#include <QtTest/QtTest>

#include "core/engine/Direction.h"
#include "core/engine/Point.h"
#include "core/engine/Grid.h"
#include "core/engine/Snake.h"
#include "core/engine/Food.h"
#include "core/engine/GameController.h"
#include "core/engine/GameConfig.h"
#include "core/engine/GameEvent.h"

using namespace core;

class TestEngine : public QObject {
    Q_OBJECT

private slots:
    void testDirectionOpposite() {
        QCOMPARE(opposite(Direction::Up), Direction::Down);
        QCOMPARE(opposite(Direction::Down), Direction::Up);
        QCOMPARE(opposite(Direction::Left), Direction::Right);
        QCOMPARE(opposite(Direction::Right), Direction::Left);
    }

    void testPointEquality() {
        Point a{1, 2};
        Point b{1, 2};
        Point c{2, 1};
        QVERIFY(a == b);
        QVERIFY(a != c);
    }

    void testPointAddition() {
        Point a{1, 2};
        Point b{3, 4};
        Point c = a + b;
        QCOMPARE(c.x, 4);
        QCOMPARE(c.y, 6);
    }

    void testGridInside() {
        Grid grid(10, 8);
        QVERIFY(grid.isInside({0, 0}));
        QVERIFY(grid.isInside({9, 7}));
        QVERIFY(!grid.isInside({-1, 0}));
        QVERIFY(!grid.isInside({0, -1}));
        QVERIFY(!grid.isInside({10, 0}));
        QVERIFY(!grid.isInside({0, 8}));
    }

    void testGridOccupy() {
        Grid grid(10, 10);
        Point p{5, 3};
        QVERIFY(!grid.isOccupied(p));
        grid.occupy(p);
        QVERIFY(grid.isOccupied(p));
        grid.clear(p);
        QVERIFY(!grid.isOccupied(p));
    }

    void testSnakeInitialState() {
        Snake snake({5, 5}, 3, Direction::Right);
        QCOMPARE(snake.length(), 3);
        QCOMPARE(snake.head().x, 5);
        QCOMPARE(snake.head().y, 5);
        QCOMPARE(snake.direction(), Direction::Right);
        QVERIFY(snake.alive());
    }

    void testSnakeMovement() {
        Snake snake({5, 5}, 3, Direction::Right);
        snake.moveOneStep();
        QCOMPARE(snake.head().x, 6);
        QCOMPARE(snake.head().y, 5);
        QCOMPARE(snake.length(), 3); // length stays same
    }

    void testSnakeGrowth() {
        Snake snake({5, 5}, 3, Direction::Right);
        snake.grow(2);
        snake.moveOneStep();
        QCOMPARE(snake.length(), 4);
        snake.moveOneStep();
        QCOMPARE(snake.length(), 5);
        snake.moveOneStep();
        QCOMPARE(snake.length(), 5);
    }

    void testSnakeCannotReverse() {
        Snake snake({5, 5}, 3, Direction::Right);
        snake.setDirection(Direction::Left); // should be ignored
        QCOMPARE(snake.direction(), Direction::Right);
        snake.setDirection(Direction::Up);
        QCOMPARE(snake.direction(), Direction::Up);
    }

    void testFoodRespawn() {
        GameConfig config;
        config.gridWidth = 10;
        config.gridHeight = 10;
        Grid grid(10, 10);
        Snake snake({5, 5}, 3, Direction::Right);
        Food food;
        food.respawn(grid, snake);
        Point pos = food.position();
        QVERIFY(grid.isInside(pos));
        QVERIFY(food.value() >= 1 && food.value() <= 3);
    }

    void testGameControllerStart() {
        GameConfig config;
        config.gridWidth = 20;
        config.gridHeight = 15;
        GameController controller(config);

        QCOMPARE(controller.score(), 0);
        QVERIFY(!controller.isGameOver());
        QCOMPARE(controller.tickNumber(), static_cast<uint64_t>(0));
    }

    void testGameControllerTick() {
        GameController controller;
        controller.start();
        controller.tick();
        QCOMPARE(controller.tickNumber(), static_cast<uint64_t>(1));
    }

    void testGameControllerWallCollision() {
        GameConfig config;
        config.gridWidth = 5;
        config.gridHeight = 5;
        // Snake starts at center (2,2), heading Right
        GameController controller(config);
        controller.start();

        // Move 3 times to hit the right wall (head at 5,2 which is out of bounds)
        for (int i = 0; i < 3; ++i) {
            controller.tick();
        }
        QVERIFY(controller.isGameOver());
    }

    void testObserverPattern() {
        GameController controller;
        controller.start();

        struct TestObserver : GameObserver {
            int tickCount = 0;
            void onEvent(const GameEvent& event) override {
                if (event.type == GameEventType::Tick) {
                    ++tickCount;
                }
            }
        };

        TestObserver obs;
        controller.addObserver(&obs);
        controller.tick();
        controller.tick();
        controller.tick();
        QCOMPARE(obs.tickCount, 3);

        controller.removeObserver(&obs);
        controller.tick();
        QCOMPARE(obs.tickCount, 3); // no more notifications
    }

    void testPauseResume() {
        GameController controller;
        controller.start();
        controller.tick();
        QCOMPARE(controller.tickNumber(), static_cast<uint64_t>(1));

        controller.pause();
        controller.tick();
        QCOMPARE(controller.tickNumber(), static_cast<uint64_t>(1)); // unchanged
    }
};

QTEST_MAIN(TestEngine)
#include "test_engine.moc"
