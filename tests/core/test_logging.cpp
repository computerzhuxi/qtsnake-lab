#include <QtTest/QtTest>

#include "core/logging/LogLevel.h"
#include "core/logging/LogSink.h"
#include "core/logging/ConsoleSink.h"
#include "core/logging/FileSink.h"
#include "core/logging/Logger.h"

#include <fstream>
#include <sstream>

using namespace core;

// -- Test sink: captures output in memory for verification --
class TestSink : public LogSink {
public:
    void write(LogLevel level, const std::string& message) override {
        Q_UNUSED(level);
        messages.push_back(message);
    }

    std::vector<std::string> messages;
};

class TestLogging : public QObject {
    Q_OBJECT

private slots:
    void testLogLevelNames() {
        QCOMPARE(QString(logLevelName(LogLevel::Debug)),   QString("DEBUG"));
        QCOMPARE(QString(logLevelName(LogLevel::Info)),    QString("INFO"));
        QCOMPARE(QString(logLevelName(LogLevel::Warning)), QString("WARN"));
        QCOMPARE(QString(logLevelName(LogLevel::Error)),   QString("ERROR"));
    }

    void testLogLevelOrdering() {
        // Debug < Info < Warning < Error
        QVERIFY(LogLevel::Debug   < LogLevel::Info);
        QVERIFY(LogLevel::Info    < LogLevel::Warning);
        QVERIFY(LogLevel::Warning < LogLevel::Error);
    }

    void testMinLevelFiltering() {
        auto sink = std::make_unique<TestSink>();
        TestSink* sinkPtr = sink.get();

        Logger::instance().addSink(std::move(sink));
        Logger::instance().setMinLevel(LogLevel::Warning);

        LOG_DEBUG("debug msg");   // should be filtered out
        LOG_INFO("info msg");     // should be filtered out
        LOG_WARN("warn msg");     // should pass
        LOG_ERROR("error msg");   // should pass

        QCOMPARE(sinkPtr->messages.size(), static_cast<size_t>(2));

        // Reset to default
        Logger::instance().setMinLevel(LogLevel::Debug);
    }

    void testMessageFormat() {
        auto sink = std::make_unique<TestSink>();
        TestSink* sinkPtr = sink.get();

        Logger::instance().addSink(std::move(sink));

        LOG_INFO("Hello World");

        QCOMPARE(sinkPtr->messages.size(), static_cast<size_t>(1));
        const std::string& msg = sinkPtr->messages[0];

        // Format: [INFO] [test_logging.cpp:line] Hello World
        QVERIFY(msg.find("[INFO]") != std::string::npos);
        QVERIFY(msg.find("[test_logging.cpp:") != std::string::npos);
        QVERIFY(msg.find("Hello World") != std::string::npos);
    }

    void testMacroLevels() {
        auto sink = std::make_unique<TestSink>();
        TestSink* sinkPtr = sink.get();

        Logger::instance().addSink(std::move(sink));
        Logger::instance().setMinLevel(LogLevel::Debug);

        LOG_DEBUG("d");
        LOG_INFO("i");
        LOG_WARN("w");
        LOG_ERROR("e");

        QCOMPARE(sinkPtr->messages.size(), static_cast<size_t>(4));
        QVERIFY(sinkPtr->messages[0].find("[DEBUG]") != std::string::npos);
        QVERIFY(sinkPtr->messages[1].find("[INFO]")  != std::string::npos);
        QVERIFY(sinkPtr->messages[2].find("[WARN]")  != std::string::npos);
        QVERIFY(sinkPtr->messages[3].find("[ERROR]") != std::string::npos);
    }

    void testConsoleSinkDoesNotCrash() {
        // Just verify we can create and write to ConsoleSink
        ConsoleSink sink;
        sink.write(LogLevel::Info, "[INFO] [test.cpp:1] test message");
        // No assertion needed — if it doesn't crash, it passes.
        QVERIFY(true);
    }

    void testFileSinkWritesToFile() {
        std::string path = "test_log_output.txt";

        {
            FileSink sink(path);
            sink.write(LogLevel::Info, "[INFO] [test.cpp:1] file log test");
        } // sink closes file on destruction

        // Verify file exists and has content
        std::ifstream file(path);
        QVERIFY(file.is_open());

        std::string line;
        std::getline(file, line);
        QVERIFY(line.find("file log test") != std::string::npos);

        file.close();
        std::remove(path.c_str());
    }

    void testSingletonReturnsSameInstance() {
        Logger& a = Logger::instance();
        Logger& b = Logger::instance();
        QCOMPARE(&a, &b);
    }

    void testThreadSafetySmoke() {
        // Smoke test: write from multiple threads
        auto sink = std::make_unique<TestSink>();
        TestSink* sinkPtr = sink.get();
        Logger::instance().addSink(std::move(sink));

        std::thread t1([]() {
            for (int i = 0; i < 100; ++i) {
                LOG_INFO("thread1");
            }
        });
        std::thread t2([]() {
            for (int i = 0; i < 100; ++i) {
                LOG_INFO("thread2");
            }
        });

        t1.join();
        t2.join();

        // All 200 messages should be captured
        QCOMPARE(sinkPtr->messages.size(), static_cast<size_t>(200));
    }
};

QTEST_MAIN(TestLogging)
#include "test_logging.moc"
