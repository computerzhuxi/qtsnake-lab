#ifndef SNAKE_LOGGING_LOGGER_H
#define SNAKE_LOGGING_LOGGER_H

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <ctime>
#include <iomanip>

/// \brief 日志级别：Debug < Info < Warning < Error
enum class LogLevel { Debug, Info, Warning, Error };

/// \brief 单例日志系统，支持控制台+文件双输出、级别过滤、时间戳
class Logger {
public:
    static Logger& instance();
    void setFile(const std::string& filepath);
    void setMinLevel(LogLevel level);
    void log(LogLevel level, const std::string& tag, const std::string& msg);

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

private:
    Logger() = default;
    LogLevel m_minLevel = LogLevel::Debug;
    std::ofstream m_file;
    bool m_hasFile = false;
    std::string levelStr(LogLevel level);
    std::string timestamp();
};

#define LOG_DEBUG(tag, msg) Logger::instance().log(LogLevel::Debug, tag, msg)
#define LOG_INFO(tag, msg)  Logger::instance().log(LogLevel::Info,  tag, msg)
#define LOG_WARN(tag, msg)  Logger::instance().log(LogLevel::Warning, tag, msg)
#define LOG_ERROR(tag, msg) Logger::instance().log(LogLevel::Error, tag, msg)

#endif // SNAKE_LOGGING_LOGGER_H
