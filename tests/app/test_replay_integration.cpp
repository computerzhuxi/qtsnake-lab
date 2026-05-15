#include <QtTest/QtTest>

#include "app/GameControllerAdapter.h"
#include "app/replay/ReplayManager.h"

#include "core/engine/GameController.h"
#include "core/engine/GameConfig.h"
#include "core/engine/Point.h"

#include <cstdio>

class TestReplayIntegration : public QObject {
    Q_OBJECT

private slots:
    void testRecordAndAutoStop() {
        core::GameConfig config;
        config.gridWidth = 5;
        config.gridHeight = 5;

        GameControllerAdapter adapter(config);
        ReplayManager replayMgr;

        replayMgr.startRecording(adapter);
        QVERIFY(replayMgr.isRecording());

        // Simulate direction at tick 1
        adapter.controller().setDirection(core::Direction::Up);

        adapter.startGame();
        for (int i = 0; i < 5; ++i) {
            adapter.controller().tick();
        }
        QVERIFY(adapter.controller().isGameOver());

        replayMgr.stopRecording();
        QVERIFY(!replayMgr.isRecording());

        std::string path = "test_integration.snake_replay";
        bool saved = replayMgr.saveToFile(QString::fromStdString(path));
        QVERIFY(saved);

        QFileInfo fi(QString::fromStdString(path));
        QVERIFY(fi.exists());
        QVERIFY(fi.size() > 0);

        bool loaded = replayMgr.loadFromFile(QString::fromStdString(path));
        QVERIFY(loaded);
        QVERIFY(replayMgr.player().data().initialSnake.size() > 0);

        std::remove(path.c_str());
    }

    void testPlaybackReconstructsGame() {
        core::GameConfig config;
        config.gridWidth = 20;
        config.gridHeight = 15;
        config.initialSnakeLength = 3;

        GameControllerAdapter adapter1(config);
        ReplayManager replayMgr;

        // Start game first (resets state), THEN record
        adapter1.startGame();
        adapter1.stopTickTimer(); // we drive ticks manually
        replayMgr.startRecording(adapter1);

        adapter1.controller().setDirection(core::Direction::Down);
        for (int i = 0; i < 10; ++i) {
            adapter1.controller().tick();
        }
        int finalScore1 = adapter1.controller().score();
        core::Point head1 = adapter1.controller().snake().head();

        replayMgr.stopRecording();

        std::string path = "test_reconstruct.snake_replay";
        replayMgr.saveToFile(QString::fromStdString(path));

        // Load and play back
        ReplayManager replayMgr2;
        replayMgr2.loadFromFile(QString::fromStdString(path));

        GameControllerAdapter adapter2(replayMgr2.player().config());
        replayMgr2.player().setupController(adapter2.controller());
        adapter2.controller().start();

        const auto& player = replayMgr2.player();
        for (uint64_t t = 1; t <= player.totalTicks(); ++t) {
            auto d = player.directionAt(t);
            if (d.has_value()) adapter2.controller().setDirection(d.value());
            adapter2.controller().tick();
        }

        QCOMPARE(adapter2.controller().snake().head().x, head1.x);
        QCOMPARE(adapter2.controller().snake().head().y, head1.y);
        QCOMPARE(adapter2.controller().score(), finalScore1);

        std::remove(path.c_str());
    }

    void testDirectionInputtedSignal() {
        core::GameConfig config;
        GameControllerAdapter adapter(config);

        bool signalReceived = false;
        core::Direction receivedDir = core::Direction::Right;

        connect(&adapter, &GameControllerAdapter::directionInputted,
                this, [&](core::Direction d) {
            signalReceived = true;
            receivedDir = d;
        });

        adapter.startGame();
        adapter.handleInput(core::Direction::Up);
        adapter.stateMachine().update();

        QVERIFY(signalReceived);
        QCOMPARE(receivedDir, core::Direction::Up);
    }
};

QTEST_MAIN(TestReplayIntegration)
#include "test_replay_integration.moc"
