#include <gtest/gtest.h>
#include "Snake.h"
#include "Point.h"
#include "Direction.h"

TEST(Snake, initialLengthIsThree) {
    Snake snake;
    EXPECT_EQ(snake.body().size(), 3);
}

TEST(Snake, initialDirectionIsRight) {
    Snake snake;
    EXPECT_EQ(snake.direction(), Direction::Right);
}

TEST(Snake, customInitialDirection) {
    Snake snake(Point{5, 5}, Direction::Up);
    EXPECT_EQ(snake.direction(), Direction::Up);
}

TEST(Snake, bodyOppositeToDirection) {
    Snake snake(Point{5, 5}, Direction::Right);
    EXPECT_EQ(snake.head(), Point(5, 5));
    EXPECT_EQ(snake.body()[1], Point(4, 5));
    EXPECT_EQ(snake.body()[2], Point(3, 5));
}

TEST(Snake, bodyOppositeToDirectionUp) {
    Snake snake(Point{5, 5}, Direction::Up);
    EXPECT_EQ(snake.head(), Point(5, 5));
    EXPECT_EQ(snake.body()[1], Point(5, 6));
    EXPECT_EQ(snake.body()[2], Point(5, 7));
}

TEST(Snake, bodyOppositeToDirectionDown) {
    Snake snake(Point{5, 5}, Direction::Down);
    EXPECT_EQ(snake.head(), Point(5, 5));
    EXPECT_EQ(snake.body()[1], Point(5, 4));
    EXPECT_EQ(snake.body()[2], Point(5, 3));
}

TEST(Snake, bodyOppositeToDirectionLeft) {
    Snake snake(Point{5, 5}, Direction::Left);
    EXPECT_EQ(snake.head(), Point(5, 5));
    EXPECT_EQ(snake.body()[1], Point(6, 5));
    EXPECT_EQ(snake.body()[2], Point(7, 5));
}

TEST(Snake, moveUpdatesPositions) {
    Snake snake(Point{5, 5});
    snake.move();
    EXPECT_EQ(snake.head(), Point(6, 5));
}

TEST(Snake, moveUp) {
    Snake snake(Point{5, 5}, Direction::Up);
    snake.move();
    EXPECT_EQ(snake.head(), Point(5, 4));
}

TEST(Snake, moveLeft) {
    Snake snake(Point{5, 5});
    snake.setDirection(Direction::Up);
    snake.move();
    snake.setDirection(Direction::Left);
    snake.move();
    EXPECT_EQ(snake.head(), Point(4, 4));
}

TEST(Snake, moveDown) {
    Snake snake(Point{5, 5}, Direction::Down);
    snake.move();
    EXPECT_EQ(snake.head(), Point(5, 6));
}

TEST(Snake, bodyFollowsHeadAfterMoves) {
    Snake snake(Point{5, 5});
    snake.move();
    EXPECT_EQ(snake.body()[0], Point(6, 5));
    EXPECT_EQ(snake.body()[1], Point(5, 5));
    EXPECT_EQ(snake.body()[2], Point(4, 5));
}

TEST(Snake, cannotReverseToOppositeDirection) {
    Snake snake(Point{5, 5}, Direction::Up);
    EXPECT_EQ(snake.direction(), Direction::Up);
    snake.setDirection(Direction::Down);
    EXPECT_EQ(snake.direction(), Direction::Up);
}

TEST(Snake, growIncreasesLength) {
    Snake snake(Point{5, 5});
    size_t oldLen = snake.body().size();
    snake.grow();
    snake.move();
    EXPECT_EQ(snake.body().size(), oldLen + 1);
}
