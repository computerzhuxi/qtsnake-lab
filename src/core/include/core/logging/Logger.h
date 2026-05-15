#ifndef CORE_LOGGING_LOGGER_H
#define CORE_LOGGING_LOGGER_H

#include "core/logging/LogLevel.h"
#include "core/logging/LogSink.h"
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace core {

class Logger {
public:
    static Logger& instance();

    void addSink(std::unique_ptr<LogSink> sink);
    void setMinLevel(LogLevel level);
    LogLevel minLevel() const { return minLevel_; }

    void log(LogLevel level, const char* file, int line,
             const std::string& message);

private:
    Logger() = default;

    std::vector<std::unique_ptr<LogSink>> sinks_;
    LogLevel minLevel_ = LogLevel::Debug;
    std::mutex mutex_;
};

} // namespace core

// Convenience macros — capture __FILE__ and __LINE__ automatically
#define LOG_DEBUG(msg) \
    core::Logger::instance().log(core::LogLevel::Debug, __FILE__, __LINE__, msg)

#define LOG_INFO(msg) \
    core::Logger::instance().log(core::LogLevel::Info, __FILE__, __LINE__, msg)

#define LOG_WARN(msg) \
    core::Logger::instance().log(core::LogLevel::Warning, __FILE__, __LINE__, msg)

#define LOG_ERROR(msg) \
    core::Logger::instance().log(core::LogLevel::Error, __FILE__, __LINE__, msg)

#endif // CORE_LOGGING_LOGGER_H
