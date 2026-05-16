#include <gtest/gtest.h>
#include "Board.h"
#include "Snake.h"
#include "Point.h"
#include "Direction.h"

class BoardTest : public ::testing::Test {
protected:
    void SetUp() override {
        board = new Board(20, 20);
        snake = new Snake(Point{10, 10});
    }
    void TearDown() override {
        delete board;
        delete snake;
    }
    Board* board;
    Snake* snake;
};

TEST_F(BoardTest, initializesWithGivenSize) {
    EXPECT_EQ(board->width(), 20);
    EXPECT_EQ(board->height(), 20);
}

TEST_F(BoardTest, snakeWithinBoundsIsAlive) {
    EXPECT_FALSE(board->checkWallCollision(*snake));
}

TEST_F(BoardTest, snakeHitsLeftWall) {
    snake->setDirection(Direction::Left);
    for (int i = 0; i < 12; ++i) snake->move();
    EXPECT_TRUE(board->checkWallCollision(*snake));
}

TEST_F(BoardTest, foodPlacedWithinBoard) {
    board->spawnFood();
    Food food = board->food();
    EXPECT_GE(food.position.x, 0);
    EXPECT_LT(food.position.x, board->width());
    EXPECT_GE(food.position.y, 0);
    EXPECT_LT(food.position.y, board->height());
}

TEST_F(BoardTest, foodNotOnSnakeBody) {
    snake->move(); snake->move(); snake->move();
    board->spawnFood(snake);
    Food food = board->food();
    for (const auto& seg : snake->body()) {
        EXPECT_FALSE(food.position == seg);
    }
}

TEST_F(BoardTest, checkFoodCollisionWhenHeadOnFood) {
    board->spawnFood();
    Food f = board->food();
    Snake s(Point{f.position.x, f.position.y});
    EXPECT_TRUE(board->checkFoodCollision(s));
}

TEST_F(BoardTest, seedProducesSameFood) {
    Board b1(20, 20, 42);
    Board b2(20, 20, 42);
    b1.spawnFood();
    b2.spawnFood();
    EXPECT_EQ(b1.food().position, b2.food().position);
}
