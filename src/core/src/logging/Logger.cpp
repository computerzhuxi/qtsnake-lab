#include "core/logging/Logger.h"
#include <sstream>

namespace core {

Logger& Logger::instance() {
    static Logger logger;
    return logger;
}

void Logger::addSink(std::unique_ptr<LogSink> sink) {
    std::lock_guard<std::mutex> lock(mutex_);
    sinks_.push_back(std::move(sink));
}

void Logger::setMinLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(mutex_);
    minLevel_ = level;
}

void Logger::log(LogLevel level, const char* file, int line,
                 const std::string& message) {
    if (level < minLevel_) return;

    // Extract filename without path
    const char* filename = file;
    const char* lastSep = file;
    for (const char* p = file; *p != '\0'; ++p) {
        if (*p == '/' || *p == '\\') {
            lastSep = p + 1;
        }
    }
    filename = lastSep;

    // Format: [LEVEL] [file:line] message
    std::ostringstream formatted;
    formatted << "[" << logLevelName(level) << "] "
              << "[" << filename << ":" << line << "] "
              << message;

    std::string msg = formatted.str();

    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& sink : sinks_) {
        sink->write(level, msg);
    }
}

} // namespace core
