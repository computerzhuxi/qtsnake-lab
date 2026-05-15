#include "core/logging/LogLevel.h"

namespace core {

const char* logLevelName(LogLevel level) {
    switch (level) {
    case LogLevel::Debug:   return "DEBUG";
    case LogLevel::Info:    return "INFO";
    case LogLevel::Warning: return "WARN";
    case LogLevel::Error:   return "ERROR";
    }
    return "UNKNOWN";
}

} // namespace core
