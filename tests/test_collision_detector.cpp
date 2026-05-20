#include <gtest/gtest.h>
#include "NaiveCollisionDetector.h"
#include "GridCollisionDetector.h"
#include "GameState.h"
#include <random>

// ========== helpers ==========

static NaiveCollisionDetector detector;

// ========== noCollision ==========

TEST(CollisionDetector, noCollision) {
    GameState s;
    s.board.setSize(10, 10);
    s.snakes.emplace_back(Point{5, 5});
    s.board.setFood(Point{8, 8});

    auto reports = detector.detect(s);
    EXPECT_TRUE(reports.empty());
}

// ========== wallCollision ==========

TEST(CollisionDetector, wallCollision) {
    GameState s;
    s.board.setSize(10, 10);
    s.snakes.emplace_back(Point{-1, 3});
    s.board.setFood(Point{5, 5});

    auto reports = detector.detect(s);
    ASSERT_EQ(reports.size(), 1u);
    EXPECT_EQ(reports[0].type, CollisionType::Wall);
    EXPECT_EQ(reports[0].snakeIndex, 0);
    EXPECT_EQ(reports[0].position, Point(-1, 3));
}

// ========== selfBodyCollision ==========

TEST(CollisionDetector, selfBodyCollision) {
    GameState s;
    s.board.setSize(10, 10);

    Snake snake(Point{2, 2}, Direction::Right);
    snake.grow();
    snake.move();
    snake.grow();
    snake.setDirection(Direction::Down);
    snake.move();
    snake.setDirection(Direction::Left);
    snake.move();
    snake.setDirection(Direction::Up);
    snake.move();
    // body = {(2,2), (2,3), (3,3), (3,2), (2,2)} → head(0) == body[4]

    s.snakes.push_back(std::move(snake));
    s.board.setFood(Point{8, 8});

    auto reports = detector.detect(s);
    ASSERT_EQ(reports.size(), 1u);
    EXPECT_EQ(reports[0].type, CollisionType::SelfBody);
    EXPECT_EQ(reports[0].snakeIndex, 0);
    EXPECT_EQ(reports[0].position, Point(2, 2));
}

// ========== otherBodyCollision ==========

TEST(CollisionDetector, otherBodyCollision) {
    GameState s;
    s.board.setSize(10, 10);

    s.snakes.emplace_back(Point{5, 5});

    Snake snake1(Point{4, 5}, Direction::Right);
    snake1.grow();
    snake1.move();
    snake1.setDirection(Direction::Up);
    snake1.move();
    // body = {(5,4), (5,5), (4,5), (3,5)}
    s.snakes.push_back(std::move(snake1));

    s.board.setFood(Point{8, 8});

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
    s.board.setSize(10, 10);

    s.snakes.emplace_back(Point{7, 7});
    s.snakes.emplace_back(Point{7, 7});
    s.board.setFood(Point{0, 0});

    auto reports = detector.detect(s);
    ASSERT_EQ(reports.size(), 2u);

    for (const auto& r : reports) {
        EXPECT_EQ(r.type, CollisionType::HeadToHead);
        EXPECT_EQ(r.position, Point(7, 7));
    }
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
    s.board.setSize(10, 10);

    s.snakes.emplace_back(Point{5, 5});
    s.board.setFood(Point{5, 5});

    auto reports = detector.detect(s);
    ASSERT_EQ(reports.size(), 1u);
    EXPECT_EQ(reports[0].type, CollisionType::Food);
    EXPECT_EQ(reports[0].snakeIndex, 0);
    EXPECT_EQ(reports[0].position, Point(5, 5));
}

// ========== obstacleCollision ==========

TEST(CollisionDetector, obstacleCollision) {
    GameState s;
    s.board.setSize(10, 10);

    s.snakes.emplace_back(Point{5, 5});
    s.board.addObstacle(Point{5, 5});
    s.board.setFood(Point{8, 8});

    auto reports = detector.detect(s);
    ASSERT_EQ(reports.size(), 1u);
    EXPECT_EQ(reports[0].type, CollisionType::Obstacle);
    EXPECT_EQ(reports[0].snakeIndex, 0);
    EXPECT_EQ(reports[0].position, Point(5, 5));
}

// ========== foodPlusBodyCollision ==========

TEST(CollisionDetector, foodPlusBodyCollision) {
    GameState s;
    s.board.setSize(10, 10);

    s.snakes.emplace_back(Point{5, 5});

    Snake snake1(Point{4, 5}, Direction::Right);
    snake1.grow();
    snake1.move();
    snake1.setDirection(Direction::Up);
    snake1.move();
    // body = {(5,4), (5,5), (4,5), (3,5)}
    s.snakes.push_back(std::move(snake1));

    s.board.setFood(Point{5, 5});

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

// ========== Grid: noCollision ==========

TEST(CollisionDetector, gridNoCollision) {
    GameState s;
    s.board.setSize(10, 10);
    s.snakes.emplace_back(Point{5, 5});
    s.board.setFood(Point{8, 8});

    GridCollisionDetector grid(10, 10);
    auto reports = grid.detect(s);
    EXPECT_TRUE(reports.empty());
}

// ========== Grid: wallCollision ==========

TEST(CollisionDetector, gridWallCollision) {
    GameState s;
    s.board.setSize(10, 10);
    s.snakes.emplace_back(Point{-1, 3});
    s.board.setFood(Point{5, 5});

    GridCollisionDetector grid(10, 10);
    auto reports = grid.detect(s);
    ASSERT_EQ(reports.size(), 1u);
    EXPECT_EQ(reports[0].type, CollisionType::Wall);
    EXPECT_EQ(reports[0].snakeIndex, 0);
    EXPECT_EQ(reports[0].position, Point(-1, 3));
}

// ========== Grid: selfBodyCollision ==========

TEST(CollisionDetector, gridSelfBodyCollision) {
    GameState s;
    s.board.setSize(10, 10);

    Snake snake(Point{2, 2}, Direction::Right);
    snake.grow();
    snake.move();
    snake.grow();
    snake.setDirection(Direction::Down);
    snake.move();
    snake.setDirection(Direction::Left);
    snake.move();
    snake.setDirection(Direction::Up);
    snake.move();

    s.snakes.push_back(std::move(snake));
    s.board.setFood(Point{8, 8});

    GridCollisionDetector grid(10, 10);
    auto reports = grid.detect(s);
    ASSERT_EQ(reports.size(), 1u);
    EXPECT_EQ(reports[0].type, CollisionType::SelfBody);
    EXPECT_EQ(reports[0].snakeIndex, 0);
    EXPECT_EQ(reports[0].position, Point(2, 2));
}

// ========== Grid: otherBodyCollision ==========

TEST(CollisionDetector, gridOtherBodyCollision) {
    GameState s;
    s.board.setSize(10, 10);

    s.snakes.emplace_back(Point{5, 5});

    Snake snake1(Point{4, 5}, Direction::Right);
    snake1.grow();
    snake1.move();
    snake1.setDirection(Direction::Up);
    snake1.move();
    s.snakes.push_back(std::move(snake1));

    s.board.setFood(Point{8, 8});

    GridCollisionDetector grid(10, 10);
    auto reports = grid.detect(s);
    ASSERT_EQ(reports.size(), 1u);
    EXPECT_EQ(reports[0].type, CollisionType::OtherBody);
    EXPECT_EQ(reports[0].snakeIndex, 0);
    EXPECT_EQ(reports[0].position, Point(5, 5));
    EXPECT_EQ(reports[0].otherSnakeIndex, 1);
}

// ========== Grid: headToHead ==========

TEST(CollisionDetector, gridHeadToHead) {
    GameState s;
    s.board.setSize(10, 10);

    s.snakes.emplace_back(Point{7, 7});
    s.snakes.emplace_back(Point{7, 7});
    s.board.setFood(Point{0, 0});

    GridCollisionDetector grid(10, 10);
    auto reports = grid.detect(s);
    ASSERT_EQ(reports.size(), 2u);

    for (const auto& r : reports) {
        EXPECT_EQ(r.type, CollisionType::HeadToHead);
        EXPECT_EQ(r.position, Point(7, 7));
    }
    bool saw0 = false, saw1 = false;
    for (const auto& r : reports) {
        if (r.snakeIndex == 0 && r.otherSnakeIndex == 1) saw0 = true;
        if (r.snakeIndex == 1 && r.otherSnakeIndex == 0) saw1 = true;
    }
    EXPECT_TRUE(saw0);
    EXPECT_TRUE(saw1);
}

// ========== Grid: foodCollision ==========

TEST(CollisionDetector, gridFoodCollision) {
    GameState s;
    s.board.setSize(10, 10);

    s.snakes.emplace_back(Point{5, 5});
    s.board.setFood(Point{5, 5});

    GridCollisionDetector grid(10, 10);
    auto reports = grid.detect(s);
    ASSERT_EQ(reports.size(), 1u);
    EXPECT_EQ(reports[0].type, CollisionType::Food);
    EXPECT_EQ(reports[0].snakeIndex, 0);
    EXPECT_EQ(reports[0].position, Point(5, 5));
}

// ========== Grid: obstacleCollision ==========

TEST(CollisionDetector, gridObstacleCollision) {
    GameState s;
    s.board.setSize(10, 10);

    s.snakes.emplace_back(Point{5, 5});
    s.board.addObstacle(Point{5, 5});
    s.board.setFood(Point{8, 8});

    GridCollisionDetector grid(10, 10);
    auto reports = grid.detect(s);
    ASSERT_EQ(reports.size(), 1u);
    EXPECT_EQ(reports[0].type, CollisionType::Obstacle);
    EXPECT_EQ(reports[0].snakeIndex, 0);
    EXPECT_EQ(reports[0].position, Point(5, 5));
}

// ========== Grid: foodPlusBodyCollision ==========

TEST(CollisionDetector, gridFoodPlusBodyCollision) {
    GameState s;
    s.board.setSize(10, 10);

    s.snakes.emplace_back(Point{5, 5});

    Snake snake1(Point{4, 5}, Direction::Right);
    snake1.grow();
    snake1.move();
    snake1.setDirection(Direction::Up);
    snake1.move();
    s.snakes.push_back(std::move(snake1));

    s.board.setFood(Point{5, 5});

    GridCollisionDetector grid(10, 10);
    auto reports = grid.detect(s);
    ASSERT_EQ(reports.size(), 1u);
    EXPECT_EQ(reports[0].type, CollisionType::OtherBody);
    EXPECT_EQ(reports[0].snakeIndex, 0);
    EXPECT_EQ(reports[0].otherSnakeIndex, 1);
}

// ========== Cross-validation: Naive vs Grid ==========

TEST(CollisionDetector, gridAndNaiveConsistent) {
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> boardDist(10, 50);
    std::uniform_int_distribution<int> snakeCountDist(1, 4);
    std::uniform_int_distribution<int> dirDist(0, 3);

    NaiveCollisionDetector naive;

    for (int iter = 0; iter < 100; ++iter) {
        int w = boardDist(rng);
        int h = boardDist(rng);
        GridCollisionDetector grid(w, h);

        GameState s;
        s.board.setSize(w, h);

        int numSnakes = snakeCountDist(rng);
        std::uniform_int_distribution<int> xDist(0, w - 1);
        std::uniform_int_distribution<int> yDist(0, h - 1);

        for (int i = 0; i < numSnakes; ++i) {
            int sx = xDist(rng);
            int sy = yDist(rng);
            Direction dir = static_cast<Direction>(dirDist(rng));
            s.snakes.emplace_back(Point{sx, sy}, dir);
        }

        s.board.setFood(Point{xDist(rng), yDist(rng)});

        auto naiveReports = naive.detect(s);
        auto gridReports = grid.detect(s);

        int totalSnakes = static_cast<int>(s.snakes.size());
        for (int i = 0; i < totalSnakes; ++i) {
            auto findDeathType = [](const std::vector<CollisionReport>& reports, int snakeIdx) {
                for (const auto& r : reports) {
                    if (r.snakeIndex != snakeIdx)
                        continue;
                    if (r.type == CollisionType::Wall ||
                        r.type == CollisionType::HeadToHead ||
                        r.type == CollisionType::SelfBody ||
                        r.type == CollisionType::OtherBody) {
                        return r.type;
                    }
                }
                return CollisionType::None;
            };

            CollisionType naiveDeath = findDeathType(naiveReports, i);
            CollisionType gridDeath = findDeathType(gridReports, i);
            EXPECT_EQ(naiveDeath, gridDeath)
                << "Iter " << iter << " snake " << i
                << " naive=" << static_cast<int>(naiveDeath)
                << " grid=" << static_cast<int>(gridDeath);
        }
    }
}
