#include <QtTest/QtTest>

#include "core/engine/GameController.h"
#include "core/engine/GameConfig.h"
#include "core/state/GameState.h"
#include "core/state/MenuState.h"
#include "core/state/PlayingState.h"
#include "core/state/PausedState.h"
#include "core/state/GameOverState.h"
#include "core/state/StateMachine.h"

using namespace core;

class TestState : public QObject {
    Q_OBJECT

private slots:
    // ---- Initial state ----
    void testInitialStateIsMenu() {
        GameConfig config;
        GameController controller(config);
        StateMachine sm(controller);

        QCOMPARE(sm.currentType(), StateType::Menu);
    }

    // ---- Menu → Playing ----
    void testStartGameFromMenu() {
        GameController controller;
        StateMachine sm(controller);

        sm.startGame();
        QCOMPARE(sm.currentType(), StateType::Playing);
    }

    // ---- GameOver → Playing ----
    void testStartGameFromGameOver() {
        GameConfig config;
        config.gridWidth = 5;
        config.gridHeight = 5;
        GameController controller(config);
        StateMachine sm(controller);

        sm.startGame();
        QCOMPARE(sm.currentType(), StateType::Playing);

        // Force game over by moving into wall
        for (int i = 0; i < 3; ++i) {
            sm.update();
        }
        QCOMPARE(sm.currentType(), StateType::GameOver);

        sm.startGame();
        QCOMPARE(sm.currentType(), StateType::Playing);
    }

    // ---- Playing → Paused → Playing ----
    void testTogglePause() {
        GameController controller;
        StateMachine sm(controller);

        sm.startGame();
        sm.togglePause();
        QCOMPARE(sm.currentType(), StateType::Paused);

        sm.togglePause();
        QCOMPARE(sm.currentType(), StateType::Playing);
    }

    // ---- Paused state does not tick ----
    void testPausedDoesNotTick() {
        GameController controller;
        StateMachine sm(controller);

        sm.startGame();
        sm.togglePause();
        QCOMPARE(sm.currentType(), StateType::Paused);

        uint64_t ticksBefore = controller.tickNumber();
        sm.update();
        QCOMPARE(controller.tickNumber(), ticksBefore); // tick() was not called
    }

    // ---- Playing state does tick ----
    void testPlayingDoesTick() {
        GameController controller;
        StateMachine sm(controller);

        sm.startGame();
        sm.update();
        QCOMPARE(controller.tickNumber(), static_cast<uint64_t>(1));
    }

    // ---- Auto transition: Playing → GameOver on wall collision ----
    void testAutoGameOver() {
        GameConfig config;
        config.gridWidth = 5;
        config.gridHeight = 5;
        GameController controller(config);
        StateMachine sm(controller);

        sm.startGame();

        // Snake starts at (2,2) heading Right, hits wall at x=5
        for (int i = 0; i < 3; ++i) {
            sm.update();
        }
        QCOMPARE(sm.currentType(), StateType::GameOver);
    }

    // ---- returnToMenu from any state ----
    void testReturnToMenu() {
        GameController controller;
        StateMachine sm(controller);

        sm.startGame();
        QCOMPARE(sm.currentType(), StateType::Playing);

        sm.returnToMenu();
        QCOMPARE(sm.currentType(), StateType::Menu);
    }

    void testReturnToMenuFromPaused() {
        GameController controller;
        StateMachine sm(controller);

        sm.startGame();
        sm.togglePause();
        QCOMPARE(sm.currentType(), StateType::Paused);

        sm.returnToMenu();
        QCOMPARE(sm.currentType(), StateType::Menu);
    }

    void testReturnToMenuFromGameOver() {
        GameConfig config;
        config.gridWidth = 5;
        config.gridHeight = 5;
        GameController controller(config);
        StateMachine sm(controller);

        sm.startGame();
        for (int i = 0; i < 3; ++i) {
            sm.update();
        }
        QCOMPARE(sm.currentType(), StateType::GameOver);

        sm.returnToMenu();
        QCOMPARE(sm.currentType(), StateType::Menu);
    }

    // ---- Direction input flows through to controller (applied on next tick) ----
    void testInputInPlaying() {
        GameController controller;
        StateMachine sm(controller);

        sm.startGame();
        sm.handleInput(Direction::Up);
        sm.update(); // tick applies the buffered direction
        QCOMPARE(controller.snake().direction(), Direction::Up);
    }

    // ---- Direction input ignored in Menu ----
    void testInputIgnoredInMenu() {
        GameController controller;
        StateMachine sm(controller);

        QCOMPARE(sm.currentType(), StateType::Menu);
        sm.handleInput(Direction::Up);
        // Direction should not change — menu ignores input
        QCOMPARE(controller.snake().direction(), Direction::Right);
    }

    // ---- Direction input ignored in Paused ----
    void testInputIgnoredInPaused() {
        GameController controller;
        StateMachine sm(controller);

        sm.startGame();
        sm.handleInput(Direction::Up);
        sm.togglePause();

        Direction before = controller.snake().direction();
        sm.handleInput(Direction::Down);
        QCOMPARE(controller.snake().direction(), before);
    }

    // ---- togglePause does nothing in Menu ----
    void testTogglePauseInMenu() {
        GameController controller;
        StateMachine sm(controller);

        QCOMPARE(sm.currentType(), StateType::Menu);
        sm.togglePause();
        QCOMPARE(sm.currentType(), StateType::Menu); // unchanged
    }

    // ---- togglePause does nothing in GameOver ----
    void testTogglePauseInGameOver() {
        GameConfig config;
        config.gridWidth = 5;
        config.gridHeight = 5;
        GameController controller(config);
        StateMachine sm(controller);

        sm.startGame();
        for (int i = 0; i < 3; ++i) {
            sm.update();
        }
        QCOMPARE(sm.currentType(), StateType::GameOver);

        sm.togglePause();
        QCOMPARE(sm.currentType(), StateType::GameOver); // unchanged
    }

    // ---- Each state reports correct type ----
    void testStateTypes() {
        MenuState menuState;
        QCOMPARE(menuState.type(), StateType::Menu);

        PlayingState playingState;
        QCOMPARE(playingState.type(), StateType::Playing);

        PausedState pausedState;
        QCOMPARE(pausedState.type(), StateType::Paused);

        GameOverState gameOverState;
        QCOMPARE(gameOverState.type(), StateType::GameOver);
    }
};

QTEST_MAIN(TestState)
#include "test_state.moc"
