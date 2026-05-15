#ifndef CORE_LOGGING_LOGLEVEL_H
#define CORE_LOGGING_LOGLEVEL_H

namespace core {

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error
};

const char* logLevelName(LogLevel level);

} // namespace core

#endif // CORE_LOGGING_LOGLEVEL_H
