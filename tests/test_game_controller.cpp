#include <gtest/gtest.h>
#include <QApplication>
#include <QTimer>
#include <QEventLoop>
#include <QVector>
#include "GameController.h"
#include "GameScene.h"

// ========== Qt 测试环境（::testing::Environment） ==========

class QtEnvironment : public ::testing::Environment {
public:
    void SetUp() override {
        static int argc = 1;
        static char name[] = "test_core";
        static char* argv[] = {name, nullptr};
        m_app = new QApplication(argc, argv);
    }
    void TearDown() override { delete m_app; }

private:
    QApplication* m_app = nullptr;
};

static ::testing::Environment* const g_qt_env =
    ::testing::AddGlobalTestEnvironment(new QtEnvironment);

// ========== 辅助函数 ==========

/// \brief 推进事件循环 ms 毫秒
static void pumpFor(int ms) {
    QEventLoop loop;
    QTimer::singleShot(ms, &loop, &QEventLoop::quit);
    loop.exec();
}

// ========== 状态机迁移 ==========

TEST(GameController, idleToReady) {
    GameScene scene;
    GameController c(&scene);
    EXPECT_EQ(c.state(), GameController::State::Idle);
    c.startGame();
    EXPECT_EQ(c.state(), GameController::State::Ready);
}

TEST(GameController, readyToCountdown) {
    GameScene scene;
    GameController c(&scene);
    c.startGame();
    c.handleReadyKey();
    EXPECT_EQ(c.state(), GameController::State::Countdown);
}

TEST(GameController, countdownToPlaying) {
    GameScene scene;
    GameController c(&scene);
    c.startGame(30, 30, 200);
    c.handleReadyKey();
    pumpFor(2500);
    EXPECT_EQ(c.state(), GameController::State::Playing);
}

TEST(GameController, playingToPausedAndBack) {
    GameScene scene;
    GameController c(&scene);
    c.startGame(30, 30, 200);
    c.handleReadyKey();
    pumpFor(2500);
    EXPECT_EQ(c.state(), GameController::State::Playing);
    c.pause();
    EXPECT_EQ(c.state(), GameController::State::Paused);
    c.resume();
    EXPECT_EQ(c.state(), GameController::State::Playing);
}

TEST(GameController, pauseNoOpInNonPlaying) {
    GameScene scene;
    GameController c(&scene);
    EXPECT_EQ(c.state(), GameController::State::Idle);
    c.pause();
    EXPECT_EQ(c.state(), GameController::State::Idle);

    c.startGame();
    EXPECT_EQ(c.state(), GameController::State::Ready);
    c.pause();
    EXPECT_EQ(c.state(), GameController::State::Ready);

    c.handleReadyKey();
    EXPECT_EQ(c.state(), GameController::State::Countdown);
    c.pause();
    EXPECT_EQ(c.state(), GameController::State::Countdown);
}

TEST(GameController, resumeNoOpInNonPaused) {
    GameScene scene;
    GameController c(&scene);
    c.resume();
    EXPECT_EQ(c.state(), GameController::State::Idle);

    c.startGame();
    c.resume();
    EXPECT_EQ(c.state(), GameController::State::Ready);

    c.handleReadyKey();
    c.resume();
    EXPECT_EQ(c.state(), GameController::State::Countdown);
}

// ========== 重置与悬空保护 ==========

TEST(GameController, resetCancelsCountdown) {
    GameScene scene;
    GameController c(&scene);
    int ticks = 0;
    QObject::connect(&c, &GameController::countdownTick,
                     [&ticks](int) { ticks++; });

    c.startGame();
    c.handleReadyKey();
    EXPECT_EQ(c.state(), GameController::State::Countdown);
    c.reset();
    EXPECT_EQ(c.state(), GameController::State::Idle);

    ticks = 0;
    pumpFor(3000);
    EXPECT_LE(ticks, 1);
}

TEST(GameController, multipleHandleReadyKeyNoStack) {
    GameScene scene;
    GameController controller(&scene);

    int tickCount = 0;
    QObject::connect(&controller, &GameController::countdownTick,
                     [&tickCount](int) { tickCount++; });

    controller.startGame();
    controller.handleReadyKey();
    controller.reset();
    controller.startGame();
    controller.handleReadyKey();
    controller.handleReadyKey();
    controller.handleReadyKey();

    tickCount = 0;
    pumpFor(3000);
    EXPECT_LE(tickCount, 4);
}

// ========== 游戏结束 ==========

TEST(GameController, collisionTriggersGameOver) {
    GameScene scene;
    GameController c(&scene);
    c.setSeed(999);
    c.startGame(5, 5, 50);

    int scoreCalls = 0;
    QObject::connect(&c, &GameController::scoreChanged,
                     [&scoreCalls](int) { scoreCalls++; });

    c.handleReadyKey();
    pumpFor(2500);

    for (int i = 0; i < 300 && c.state() != GameController::State::GameOver; ++i) {
        pumpFor(60);
    }
    EXPECT_EQ(c.state(), GameController::State::GameOver);
    EXPECT_TRUE(c.gameState().gameOver);
    EXPECT_GE(scoreCalls, 1);
}

// ========== 确定性 ==========

TEST(GameController, sameSeedSameFoodSequence) {
    GameScene s1; GameController c1(&s1);
    GameScene s2; GameController c2(&s2);
    c1.setSeed(42); c2.setSeed(42);
    c1.startGame(10, 10, 100);
    c2.startGame(10, 10, 100);
    EXPECT_EQ(c1.gameState().board.foodPos, c2.gameState().board.foodPos);

    c1.handleReadyKey(); c2.handleReadyKey();
    pumpFor(3000);
    EXPECT_EQ(c1.gameState().board.foodPos, c2.gameState().board.foodPos);
}

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

TEST(GameController, setSeedIgnoredOutsideIdle) {
    GameScene scene;
    GameController c(&scene);
    c.setSeed(42);
    c.startGame(10, 10, 100);
    Point foodAfterStart = c.gameState().board.foodPos;

    c.setSeed(12345);
    EXPECT_EQ(c.gameState().board.foodPos, foodAfterStart);
}

// ========== 信号契约 ==========

TEST(GameController, stateChangedSignalEmits) {
    GameScene scene;
    GameController c(&scene);

    QVector<GameController::State> states;
    QObject::connect(&c, &GameController::stateChanged,
                     [&states](GameController::State s) { states.append(s); });

    c.startGame(30, 30, 200);
    c.handleReadyKey();
    pumpFor(2500);
    c.pause();
    c.resume();

    ASSERT_EQ(states.size(), 5);
    EXPECT_EQ(states[0], GameController::State::Ready);
    EXPECT_EQ(states[1], GameController::State::Countdown);
    EXPECT_EQ(states[2], GameController::State::Playing);
    EXPECT_EQ(states[3], GameController::State::Paused);
    EXPECT_EQ(states[4], GameController::State::Playing);
}

TEST(GameController, scoreChangedOnStartAndDeath) {
    GameScene scene;
    GameController c(&scene);

    QVector<int> scores;
    QObject::connect(&c, &GameController::scoreChanged,
                     [&scores](int s) { scores.append(s); });

    c.setSeed(777);
    c.startGame(5, 5, 50);
    ASSERT_GE(scores.size(), 1);
    EXPECT_EQ(scores[0], 0);

    int callsBefore = scores.size();

    c.handleReadyKey();
    pumpFor(2500);

    for (int i = 0; i < 300 && c.state() != GameController::State::GameOver; ++i) {
        pumpFor(60);
    }
    EXPECT_GT(scores.size(), callsBefore);
}
