#include <gtest/gtest.h>
#include <QApplication>
#include <QTimer>
#include <QEventLoop>
#include "GameController.h"
#include "GameScene.h"

static int g_argc = 1;
static char g_argv0[] = "test_core";
static char* g_argv_arr[] = {g_argv0, nullptr};
static QApplication g_app(g_argc, g_argv_arr);

/// \brief 推进事件循环 ms 毫秒（不阻塞主线程以外的操作）
static void advanceTime(int ms) {
    QEventLoop loop;
    QTimer::singleShot(ms, &loop, &QEventLoop::quit);
    loop.exec();
}

// ========== Test A: reset() stops countdown ==========

TEST(GameController, resetStopsCountdown) {
    GameScene scene;
    GameController controller(&scene);

    int tickCount = 0;
    QObject::connect(&controller, &GameController::countdownTick,
                     [&tickCount](int) { tickCount++; });

    controller.startGame();
    EXPECT_EQ(controller.state(), GameController::State::Ready);

    controller.handleReadyKey();
    EXPECT_EQ(controller.state(), GameController::State::Countdown);

    controller.reset();
    EXPECT_EQ(controller.state(), GameController::State::Idle);

    tickCount = 0;
    advanceTime(3000);
    EXPECT_EQ(tickCount, 0);
}

// ========== Test B: repeated handleReadyKey does not stack ==========

TEST(GameController, repeatedHandleReadyKeyDoesNotStack) {
    GameScene scene;
    GameController controller(&scene);

    int tickCount = 0;
    QObject::connect(&controller, &GameController::countdownTick,
                     [&tickCount](int) { tickCount++; });

    controller.startGame();

    controller.handleReadyKey();
    controller.handleReadyKey();
    controller.handleReadyKey();

    advanceTime(3000);

    EXPECT_LE(tickCount, 4);
}

// ========== Test C: setSeed produces deterministic start ==========

TEST(GameController, setSeedProducesDeterministicStart) {
    GameScene scene1;
    GameController c1(&scene1);
    c1.setSeed(42);
    c1.startGame(10, 10, 100);

    GameScene scene2;
    GameController c2(&scene2);
    c2.setSeed(42);
    c2.startGame(10, 10, 100);

    const auto& gs1 = c1.gameState();
    const auto& gs2 = c2.gameState();

    EXPECT_EQ(gs1.board.foodPos, gs2.board.foodPos);
    EXPECT_EQ(gs1.snakes[0].head(), gs2.snakes[0].head());
    EXPECT_EQ(gs1.snakes[0].body(), gs2.snakes[0].body());
}
