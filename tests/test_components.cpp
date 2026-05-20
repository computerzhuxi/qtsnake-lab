#include <gtest/gtest.h>
#include "GameState.h"
#include "components/MoveComponent.h"
#include "components/CollisionComponent.h"
#include "components/FoodComponent.h"
#include "components/InputComponent.h"
#include "RngService.h"
#include "Snake.h"

// ========== MoveComponent ==========

TEST(MoveComponent, initBuildsCorrectFreeCells) {
    GameState s;
    s.board.setSize(10, 10);
    s.snakes.emplace_back(Point{5, 5});
    s.board.initFromSnakes(s.snakes);
    EXPECT_EQ(s.board.freeCells().size(), 100 - 3);
}

TEST(MoveComponent, growDoesNotAddTail) {
    GameState s;
    s.board.setSize(10, 10);
    s.snakes.emplace_back(Point{5, 5});
    s.board.initFromSnakes(s.snakes);

    s.snakes[0].grow();
    size_t oldSize = s.board.freeCells().size();

    MoveComponent move;
    move.update(s);

    // 增长了：旧尾不加回，新头被 placeHead 移除
    EXPECT_EQ(s.board.freeCells().size(), oldSize - 1);
}

TEST(MoveComponent, updateAddsTailNotHead) {
    GameState s;
    s.board.setSize(10, 10);
    s.snakes.emplace_back(Point{5, 5});
    s.board.initFromSnakes(s.snakes);

    size_t oldSize = s.board.freeCells().size();
    Point oldTail = s.snakes[0].body().back();
    Point oldHead = s.snakes[0].head();

    MoveComponent move;
    move.update(s);

    // 旧尾加回空闲集
    EXPECT_TRUE(s.board.freeCells().count(oldTail));

    // 新头被 placeHead 移除
    EXPECT_FALSE(s.board.freeCells().count(s.snakes[0].head()));

    // 旧头移为身体段，不在空闲集
    EXPECT_FALSE(s.board.freeCells().count(oldHead));

    // 旧尾+1，新头移除-1：大小不变
    EXPECT_EQ(s.board.freeCells().size(), oldSize);
}

// ========== CollisionComponent ==========

TEST(CollisionComponent, headInFreeCellsIsSafe) {
    GameState s;
    s.board.setSize(10, 10);
    s.snakes.emplace_back(Point{5, 5});
    s.board.initFromSnakes(s.snakes);
    s.board.setFood(Point{8, 8});
    s.board.removeFreeCell(Point{8, 8});

    MoveComponent move;
    move.update(s);
    CollisionComponent coll;
    auto reports = coll.detect(s);
    EXPECT_TRUE(reports.empty());
}

TEST(CollisionComponent, headOnFoodIsSafe) {
    GameState s;
    s.board.setSize(10, 10);
    s.snakes.emplace_back(Point{5, 5});
    s.board.initFromSnakes(s.snakes);
    s.board.setFood(Point{6, 5});
    s.board.removeFreeCell(Point{6, 5});

    MoveComponent move;
    move.update(s);
    CollisionComponent coll;
    auto reports = coll.detect(s);
    ASSERT_EQ(reports.size(), 1u);
    EXPECT_EQ(reports[0].type, CollisionType::Food);
    EXPECT_EQ(reports[0].snakeIndex, 0);
}

TEST(CollisionComponent, headOutsideBoardIsDead) {
    GameState s;
    s.board.setSize(10, 10);
    s.snakes.emplace_back(Point{9, 5});
    s.board.initFromSnakes(s.snakes);
    s.board.setFood(Point{0, 0});
    s.board.removeFreeCell(Point{0, 0});

    s.snakes[0].setDirection(Direction::Right);
    MoveComponent move;
    move.update(s);
    CollisionComponent coll;
    auto reports = coll.detect(s);
    ASSERT_EQ(reports.size(), 1u);
    EXPECT_EQ(reports[0].type, CollisionType::Wall);
}

TEST(CollisionComponent, headOnSnakeBodyIsDead) {
    GameState s;
    s.board.setSize(10, 10);
    s.snakes.emplace_back(Point{5, 5});
    s.board.setFood(Point{0, 0});
    s.board.initFromSnakes(s.snakes);

    // 通过 Board API 在 (6,5) 模拟身体段占据
    s.board.placeBody(1, Point{6, 5});

    MoveComponent move;
    move.update(s);  // head → (6,5)
    CollisionComponent coll;
    auto reports = coll.detect(s);
    ASSERT_EQ(reports.size(), 1u);
    EXPECT_EQ(reports[0].type, CollisionType::OtherBody);
    EXPECT_EQ(reports[0].snakeIndex, 0);
}

// ========== FoodComponent ==========

TEST(FoodComponent, initSpawnsFoodInsideFreeCells) {
    GameState s;
    s.board.setSize(10, 10);
    s.snakes.emplace_back(Point{5, 5});
    s.board.initFromSnakes(s.snakes);
    RngService rng;
    FoodComponent food(&rng);
    food.init(s);
    EXPECT_FALSE(s.board.freeCells().count(s.board.foodPos()));
}

TEST(FoodComponent, eatingFoodGrowsSnakeAndSpawnsNew) {
    GameState s;
    s.board.setSize(10, 10);
    s.snakes.emplace_back(Point{5, 5});
    s.board.initFromSnakes(s.snakes);
    RngService rng2;
    FoodComponent foodComp(&rng2);
    foodComp.init(s);

    s.board.addFreeCell(s.board.foodPos());
    s.board.setFood(Point{6, 5});
    s.board.removeFreeCell(Point{6, 5});

    int oldLen = static_cast<int>(s.snakes[0].body().size());
    int oldScore = s.score;

    MoveComponent move;
    move.update(s);
    foodComp.update(s);

    move.update(s);
    EXPECT_GT(s.snakes[0].body().size(), oldLen);
    EXPECT_GT(s.score, oldScore);
    EXPECT_NE(s.board.foodPos(), Point(6, 5));
    EXPECT_FALSE(s.board.freeCells().count(s.board.foodPos()));
}

// ========== InputComponent ==========

TEST(InputComponent, setDirectionOverwritesCache) {
    GameState s;
    s.board.setSize(10, 10);
    s.snakes.emplace_back(Point{5, 5}, Direction::Up);

    InputComponent in;
    in.init(s);
    EXPECT_EQ(s.snakes[0].direction(), Direction::Up);

    in.setDirection(Direction::Right);
    in.update(s);
    EXPECT_EQ(s.snakes[0].direction(), Direction::Right);
}

TEST(InputComponent, snakeReverseGuardStillEffective) {
    GameState s;
    s.board.setSize(10, 10);
    s.snakes.emplace_back(Point{5, 5}, Direction::Up);

    InputComponent in;
    in.init(s);

    in.setDirection(Direction::Down);
    in.update(s);
    EXPECT_EQ(s.snakes[0].direction(), Direction::Up);
}

// ========== RngService ==========

TEST(RngService, sameSeedProducesIdenticalSequence) {
    RngService a;
    RngService b;
    a.seed(42);
    b.seed(42);
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(a.intInRange(0, 99), b.intInRange(0, 99));
    }
}
