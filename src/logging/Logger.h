#ifndef SNAKE_LOGGING_LOGGER_H
#define SNAKE_LOGGING_LOGGER_H

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>

/// \brief 日志级别：Debug < Info < Warning < Error
enum class LogLevel { Debug, Info, Warning, Error };

/// \brief 异步单例日志系统：后台线程消费消息队列，控制台+文件双输出
class Logger {
public:
    static Logger& instance();
    ~Logger();

    void setFile(const std::string& filepath);
    void setMinLevel(LogLevel level);
    void log(LogLevel level, const std::string& tag, const std::string& msg);

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

private:
    Logger();
    void workerLoop();
    void writeLine(const std::string& line);

    LogLevel m_minLevel = LogLevel::Debug;
    std::ofstream m_file;
    bool m_hasFile = false;

    std::queue<std::string> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::thread m_worker;
    std::atomic<bool> m_running{true};

    std::string levelStr(LogLevel level);
    std::string timestamp();
};

#define LOG_DEBUG(tag, msg) Logger::instance().log(LogLevel::Debug, tag, msg)
#define LOG_INFO(tag, msg)  Logger::instance().log(LogLevel::Info,  tag, msg)
#define LOG_WARN(tag, msg)  Logger::instance().log(LogLevel::Warning, tag, msg)
#define LOG_ERROR(tag, msg) Logger::instance().log(LogLevel::Error, tag, msg)

#endif // SNAKE_LOGGING_LOGGER_H
