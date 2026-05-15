#ifndef CORE_LOGGING_CONSOLESINK_H
#define CORE_LOGGING_CONSOLESINK_H

#include "core/logging/LogSink.h"

namespace core {

class ConsoleSink : public LogSink {
public:
    void write(LogLevel level, const std::string& message) override;
};

} // namespace core

#endif // CORE_LOGGING_CONSOLESINK_H
