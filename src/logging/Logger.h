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

/// \brief 异步单例日志系统
/// \details 后台线程 + 消息队列架构。
///          log() 只负责格式化 + 入队，不阻塞调用方。
///          后台线程从队列取消息写入控制台和文件。
///          析构时清空队列再退出，防止日志丢失。

class Logger {
public:
    static Logger& instance();
    ~Logger();

    void setFile(const std::string& filepath);
    void setMinLevel(LogLevel level);

    /// \brief 记录一条日志（异步：入队后立即返回）
    void log(LogLevel level, const std::string& tag, const std::string& msg);

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

private:
    Logger();

    /// \brief 工作线程主循环：等队列有消息 → 取出 → 写入
    void workerLoop();

    /// \brief 将一行日志写入控制台和文件
    void writeLine(const std::string& line);

    std::string levelStr(LogLevel level);
    std::string timestamp();

    LogLevel m_minLevel = LogLevel::Debug;  ///< 最低输出级别，低于此级别的日志被丢弃
    std::ofstream m_file;                   ///< 日志文件流（可选）
    bool m_hasFile = false;                 ///< 是否已设置文件输出

    std::queue<std::string> m_queue;        ///< 日志消息队列
    std::mutex m_mutex;                     ///< 保护队列和文件流的互斥锁
    std::condition_variable m_cv;            ///< 通知工作线程有新消息
    std::thread m_worker;                   ///< 后台工作线程
    std::atomic<bool> m_running{true};      ///< 工作线程运行标志
};

#define LOG_DEBUG(tag, msg) Logger::instance().log(LogLevel::Debug, tag, msg)
#define LOG_INFO(tag, msg)  Logger::instance().log(LogLevel::Info,  tag, msg)
#define LOG_WARN(tag, msg)  Logger::instance().log(LogLevel::Warning, tag, msg)
#define LOG_ERROR(tag, msg) Logger::instance().log(LogLevel::Error, tag, msg)

#endif // SNAKE_LOGGING_LOGGER_H
