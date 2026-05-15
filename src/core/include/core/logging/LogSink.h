#ifndef CORE_LOGGING_LOGSINK_H
#define CORE_LOGGING_LOGSINK_H

#include "core/logging/LogLevel.h"
#include <string>

namespace core {

class LogSink {
public:
    virtual ~LogSink() = default;
    virtual void write(LogLevel level, const std::string& message) = 0;
};

} // namespace core

#endif // CORE_LOGGING_LOGSINK_H
