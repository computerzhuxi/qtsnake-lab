#include <gtest/gtest.h>
#include "NaiveCollisionDetector.h"
#include "GameState.h"

// ========== helpers ==========

static NaiveCollisionDetector detector;

// ========== noCollision ==========

TEST(CollisionDetector, noCollision) {
    GameState s;
    s.board.width = 10;
    s.board.height = 10;
    s.snakes.emplace_back(Point{5, 5});
    s.board.foodPos = Point{8, 8};

    auto reports = detector.detect(s);
    EXPECT_TRUE(reports.empty());
}

// ========== wallCollision ==========

TEST(CollisionDetector, wallCollision) {
    GameState s;
    s.board.width = 10;
    s.board.height = 10;
    s.snakes.emplace_back(Point{-1, 3});
    s.board.foodPos = Point{5, 5};

    auto reports = detector.detect(s);
    ASSERT_EQ(reports.size(), 1u);
    EXPECT_EQ(reports[0].type, CollisionType::Wall);
    EXPECT_EQ(reports[0].snakeIndex, 0);
    EXPECT_EQ(reports[0].position, Point(-1, 3));
}

// ========== selfBodyCollision ==========

TEST(CollisionDetector, selfBodyCollision) {
    // 构造一条自碰蛇：head == body[4]
    // Sequence: (2,2)R → grow→R → grow→D → L → U
    // Result: {(2,2), (2,3), (3,3), (3,2), (2,2)}
    GameState s;
    s.board.width = 10;
    s.board.height = 10;

    Snake snake(Point{2, 2}, Direction::Right);
    // body = {(2,2), (1,2), (0,2)}
    snake.grow();
    snake.move();
    // body = {(3,2), (2,2), (1,2), (0,2)}
    snake.grow();
    snake.setDirection(Direction::Down);
    snake.move();
    // body = {(3,3), (3,2), (2,2), (1,2), (0,2)}
    snake.setDirection(Direction::Left);
    snake.move();
    // body = {(2,3), (3,3), (3,2), (2,2), (1,2)}
    snake.setDirection(Direction::Up);
    snake.move();
    // body = {(2,2), (2,3), (3,3), (3,2), (2,2)} → head(0) == body[4]

    s.snakes.push_back(std::move(snake));
    s.board.foodPos = Point{8, 8};

    auto reports = detector.detect(s);
    ASSERT_EQ(reports.size(), 1u);
    EXPECT_EQ(reports[0].type, CollisionType::SelfBody);
    EXPECT_EQ(reports[0].snakeIndex, 0);
    EXPECT_EQ(reports[0].position, Point(2, 2));
}

// ========== otherBodyCollision ==========

TEST(CollisionDetector, otherBodyCollision) {
    GameState s;
    s.board.width = 10;
    s.board.height = 10;

    // Snake 0: head at (5,5)
    s.snakes.emplace_back(Point{5, 5});

    // Snake 1: body contains (5,5) at body[1]
    Snake snake1(Point{4, 5}, Direction::Right);
    // body = {(4,5), (3,5), (2,5)}
    snake1.grow();
    snake1.move();
    // body = {(5,5), (4,5), (3,5), (2,5)}
    snake1.setDirection(Direction::Up);
    snake1.move();
    // body = {(5,4), (5,5), (4,5), (3,5)}
    s.snakes.push_back(std::move(snake1));

    s.board.foodPos = Point{8, 8};

    auto reports = detector.detect(s);
    ASSERT_EQ(reports.size(), 1u);
    EXPECT_EQ(reports[0].type, CollisionType::OtherBody);
    EXPECT_EQ(reports[0].snakeIndex, 0);
    EXPECT_EQ(reports[0].position, Point(5, 5));
    EXPECT_EQ(reports[0].otherSnakeIndex, 1);
}

// ========== headToHead ==========

TEST(CollisionDetector, headToHead) {
    GameState s;
    s.board.width = 10;
    s.board.height = 10;

    s.snakes.emplace_back(Point{7, 7});
    s.snakes.emplace_back(Point{7, 7});
    s.board.foodPos = Point{0, 0};

    auto reports = detector.detect(s);
    ASSERT_EQ(reports.size(), 2u);

    // 两条蛇各报 1 个 HeadToHead
    for (const auto& r : reports) {
        EXPECT_EQ(r.type, CollisionType::HeadToHead);
        EXPECT_EQ(r.position, Point(7, 7));
    }
    // snake 0 报 other=1，snake 1 报 other=0
    bool saw0 = false, saw1 = false;
    for (const auto& r : reports) {
        if (r.snakeIndex == 0 && r.otherSnakeIndex == 1) saw0 = true;
        if (r.snakeIndex == 1 && r.otherSnakeIndex == 0) saw1 = true;
    }
    EXPECT_TRUE(saw0);
    EXPECT_TRUE(saw1);
}

// ========== foodCollision ==========

TEST(CollisionDetector, foodCollision) {
    GameState s;
    s.board.width = 10;
    s.board.height = 10;

    s.snakes.emplace_back(Point{5, 5});
    s.board.foodPos = Point{5, 5};

    auto reports = detector.detect(s);
    ASSERT_EQ(reports.size(), 1u);
    EXPECT_EQ(reports[0].type, CollisionType::Food);
    EXPECT_EQ(reports[0].snakeIndex, 0);
    EXPECT_EQ(reports[0].position, Point(5, 5));
}

// ========== obstacleCollision ==========

TEST(CollisionDetector, obstacleCollision) {
    GameState s;
    s.board.width = 10;
    s.board.height = 10;

    s.snakes.emplace_back(Point{5, 5});
    s.board.obstacles.insert(Point{5, 5});
    s.board.foodPos = Point{8, 8};

    auto reports = detector.detect(s);
    ASSERT_EQ(reports.size(), 1u);
    EXPECT_EQ(reports[0].type, CollisionType::Obstacle);
    EXPECT_EQ(reports[0].snakeIndex, 0);
    EXPECT_EQ(reports[0].position, Point(5, 5));
}

// ========== foodPlusBodyCollision ==========

TEST(CollisionDetector, foodPlusBodyCollision) {
    GameState s;
    s.board.width = 10;
    s.board.height = 10;

    // Snake 0: head at (5,5)
    s.snakes.emplace_back(Point{5, 5});

    // Snake 1: body contains (5,5) at body[1]
    Snake snake1(Point{4, 5}, Direction::Right);
    snake1.grow();
    snake1.move();
    snake1.setDirection(Direction::Up);
    snake1.move();
    // body = {(5,4), (5,5), (4,5), (3,5)}
    s.snakes.push_back(std::move(snake1));

    // Food at same position as snake 0 head
    s.board.foodPos = Point{5, 5};

    auto reports = detector.detect(s);
    ASSERT_EQ(reports.size(), 2u);

    bool sawFood = false, sawOtherBody = false;
    for (const auto& r : reports) {
        if (r.type == CollisionType::Food && r.snakeIndex == 0 && r.position == Point(5, 5))
            sawFood = true;
        if (r.type == CollisionType::OtherBody && r.snakeIndex == 0 && r.otherSnakeIndex == 1)
            sawOtherBody = true;
    }
    EXPECT_TRUE(sawFood);
    EXPECT_TRUE(sawOtherBody);
}
