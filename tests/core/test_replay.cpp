#include <QtTest/QtTest>

#include "core/engine/GameController.h"
#include "core/engine/GameConfig.h"
#include "core/replay/ReplayData.h"
#include "core/replay/ReplaySerializer.h"
#include "core/replay/ReplayRecorder.h"
#include "core/replay/ReplayPlayer.h"

#include <cstdio>

using namespace core;

class TestReplay : public QObject {
    Q_OBJECT

private slots:
    void testRecorderStartsEmpty() {
        ReplayRecorder recorder;
        QVERIFY(!recorder.isRecording());
    }

    void testRecorderCapturesInitialState() {
        GameConfig config;
        config.gridWidth = 20;
        config.gridHeight = 15;
        GameController ctrl(config);

        ReplayRecorder recorder;
        recorder.start(ctrl);
        QVERIFY(recorder.isRecording());

        const auto& data = recorder.data();
        QCOMPARE(data.config.gridWidth, 20);
        QCOMPARE(data.config.gridHeight, 15);
        QCOMPARE(static_cast<int>(data.initialSnake.size()), config.initialSnakeLength);
        QVERIFY(data.rngSeed != 0u);
    }

    void testRecorderDirectionChanges() {
        GameController ctrl;
        ReplayRecorder recorder;
        recorder.start(ctrl);

        recorder.recordDirection(5, Direction::Up);
        recorder.recordDirection(10, Direction::Left);
        recorder.recordDirection(10, Direction::Right); // overwrites at same tick

        const auto& data = recorder.data();
        QCOMPARE(data.directionChanges.size(), static_cast<size_t>(2));
        QCOMPARE(data.directionChanges.at(5), Direction::Up);
        QCOMPARE(data.directionChanges.at(10), Direction::Right); // last one wins
    }

    void testRecorderFinish() {
        GameController ctrl;
        ctrl.start();
        ctrl.tick();
        ctrl.tick();
        ctrl.tick();

        ReplayRecorder recorder;
        recorder.start(ctrl);
        auto data = recorder.finish(ctrl);

        QVERIFY(!recorder.isRecording());
        QCOMPARE(data.totalTicks, ctrl.tickNumber());
        QCOMPARE(data.finalScore, ctrl.score());
    }

    void testSaveAndLoadRoundTrip() {
        GameConfig config;
        config.gridWidth  = 25;
        config.gridHeight = 18;
        config.initialSpeed = 130;
        GameController ctrl(config);

        ReplayRecorder recorder;
        recorder.start(ctrl);
        recorder.recordDirection(1, Direction::Down);
        recorder.recordDirection(6, Direction::Right);
        auto data = recorder.finish(ctrl);

        std::string path = "test_roundtrip.snake_replay";
        QVERIFY(saveReplayData(data, path));

        ReplayData loaded = loadReplayData(path);
        QCOMPARE(loaded.config.gridWidth, 25);
        QCOMPARE(loaded.config.gridHeight, 18);
        QCOMPARE(loaded.config.initialSpeed, 130);
        QCOMPARE(loaded.rngSeed, data.rngSeed);
        QCOMPARE(loaded.initialFood.x, data.initialFood.x);
        QCOMPARE(loaded.initialFood.y, data.initialFood.y);
        QCOMPARE(static_cast<int>(loaded.initialSnake.size()),
                 static_cast<int>(data.initialSnake.size()));
        QCOMPARE(loaded.directionChanges.size(), static_cast<size_t>(2));
        QCOMPARE(loaded.directionChanges.at(1), Direction::Down);
        QCOMPARE(loaded.directionChanges.at(6), Direction::Right);

        std::remove(path.c_str());
    }

    void testPlayerLoadAndPlayback() {
        GameConfig config;
        config.gridWidth  = 20;
        config.gridHeight = 15;
        GameController ctrl(config);
        ctrl.start();
        unsigned int seed = ctrl.rngSeed();
        ReplayRecorder recorder;
        recorder.start(ctrl);

        // Simulate: direction Down at tick 1, tick 2, tick 3, tick 4, tick 5
        recorder.recordDirection(1, Direction::Down);
        auto data = recorder.finish(ctrl);

        std::string path = "test_player.snake_replay";
        saveReplayData(data, path);

        ReplayPlayer player;
        QVERIFY(player.loadFromFile(path));
        QCOMPARE(player.totalTicks(), ctrl.tickNumber());

        auto d1 = player.directionAt(1);
        QVERIFY(d1.has_value());
        QCOMPARE(d1.value(), Direction::Down);

        auto d2 = player.directionAt(2);
        QVERIFY(!d2.has_value()); // no direction change at tick 2

        std::remove(path.c_str());
    }

    void testDeterministicReplay() {
        GameConfig config;
        config.gridWidth  = 30;
        config.gridHeight = 20;
        config.initialSnakeLength = 3;

        // Original game
        GameController c1(config);
        c1.start();

        ReplayRecorder recorder;
        recorder.start(c1);
        recorder.recordDirection(1, Direction::Down);
        recorder.recordDirection(6, Direction::Right);

        for (int i = 1; i <= 8; ++i) {
            if (i == 1) c1.setDirection(Direction::Down);
            if (i == 6) c1.setDirection(Direction::Right);
            c1.tick();
        }
        auto data = recorder.finish(c1);

        std::string path = "test_det.snake_replay";
        saveReplayData(data, path);

        // Replay
        ReplayPlayer player;
        QVERIFY(player.loadFromFile(path));

        GameController c2(player.config());
        player.setupController(c2);
        c2.start();

        for (uint64_t t = 1; t <= player.totalTicks(); ++t) {
            auto d = player.directionAt(t);
            if (d.has_value()) c2.setDirection(d.value());
            c2.tick();
        }

        QCOMPARE(c2.snake().head().x, c1.snake().head().x);
        QCOMPARE(c2.snake().head().y, c1.snake().head().y);
        QCOMPARE(c2.snake().length(), c1.snake().length());
        QCOMPARE(c2.score(), c1.score());

        std::remove(path.c_str());
    }

    void testTextFormatIsReadable() {
        GameConfig config;
        GameController ctrl(config);

        ReplayRecorder recorder;
        recorder.start(ctrl);
        auto data = recorder.finish(ctrl);

        std::string path = "test_readable.snake_replay";
        saveReplayData(data, path);

        // Verify it's plain text with expected sections
        QFile file(QString::fromStdString(path));
        QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
        QString content = QString::fromUtf8(file.readAll());
        file.close();

        QVERIFY(content.contains("[config]"));
        QVERIFY(content.contains("[initial]"));
        QVERIFY(content.contains("[result]"));
        QVERIFY(content.contains("rng_seed="));
        QVERIFY(content.contains("snake="));
        QVERIFY(content.contains("food="));

        std::remove(path.c_str());
    }
};

QTEST_MAIN(TestReplay)
#include "test_replay.moc"
