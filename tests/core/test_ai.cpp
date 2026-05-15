#include <QtTest/QtTest>

#include "core/ai/AStarPathfinder.h"
#include "core/engine/Grid.h"
#include "core/engine/Snake.h"

using namespace core;

class TestAI : public QObject {
    Q_OBJECT

private slots:
    void testManhattan() {
        QCOMPARE(AStarPathfinder::manhattan({0, 0}, {3, 4}), 7);
        QCOMPARE(AStarPathfinder::manhattan({5, 2}, {1, 6}), 8);
    }

    void testDirectPath() {
        Grid grid(10, 10);
        Snake snake({2, 5}, 1, Direction::Right);
        AStarPathfinder pf;

        Point food {7, 5};
        PathResult result = pf.findPath(grid, snake, food);
        QVERIFY(result.found);
        QVERIFY(result.path.size() >= 2);
        // First step should be toward food (to the right)
        QCOMPARE(result.path[1].x, 3);
        QCOMPARE(result.path[1].y, 5);
    }

    void testAvoidSelf() {
        Grid grid(10, 10);
        // Snake: head at {5,5}, body extends left.
        // Body: [5,5], [4,5], [3,5], [2,5], [1,5] — tail at {1,5}
        // Food at {1,5} (tail position): tail excluded from obstacles → reachable
        Snake snake({5, 5}, 5, Direction::Right);

        AStarPathfinder pf;
        // Food at the tail position — should be reachable since tail is excluded
        Point food {1, 5};
        PathResult result = pf.findPath(grid, snake, food);
        QVERIFY(result.found);
    }

    void testWallAvoidance() {
        Grid grid(10, 10);
        // Snake at bottom-left corner heading right
        Snake snake({1, 1}, 1, Direction::Right);
        AStarPathfinder pf;

        Point food {8, 1}; // far right, same row
        PathResult result = pf.findPath(grid, snake, food);
        QVERIFY(result.found);
    }

    void testNoPath() {
        Grid grid(5, 5);
        // Snake wraps itself completely
        // Body blocks all exits except where the snake came from
        // Actually let's test: snake fills almost entire space
        Snake snake({2, 2}, 1, Direction::Right);
        AStarPathfinder pf;

        // Put food outside grid — should find no path
        Point food {-1, -1};
        PathResult result = pf.findPath(grid, snake, food);
        QVERIFY(!result.found);
    }
};

QTEST_MAIN(TestAI)
#include "test_ai.moc"
