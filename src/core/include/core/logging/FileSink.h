#ifndef CORE_LOGGING_FILESINK_H
#define CORE_LOGGING_FILESINK_H

#include "core/logging/LogSink.h"
#include <fstream>
#include <string>

namespace core {

class FileSink : public LogSink {
public:
    explicit FileSink(const std::string& filepath);
    ~FileSink() override;

    void write(LogLevel level, const std::string& message) override;

private:
    std::ofstream file_;
};

} // namespace core

#endif // CORE_LOGGING_FILESINK_H
