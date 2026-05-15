#include "core/logging/ConsoleSink.h"
#include <iostream>

namespace core {

void ConsoleSink::write(LogLevel level, const std::string& message) {
    if (level == LogLevel::Warning || level == LogLevel::Error) {
        std::cerr << message << std::endl;
    } else {
        std::cout << message << std::endl;
    }
}

} // namespace core
