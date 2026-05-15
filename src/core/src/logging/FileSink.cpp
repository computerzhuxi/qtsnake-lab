#include "core/logging/FileSink.h"
#include <chrono>
#include <ctime>
#include <iomanip>

namespace core {

FileSink::FileSink(const std::string& filepath)
    : file_(filepath, std::ios::out | std::ios::app)
{
}

FileSink::~FileSink() {
    if (file_.is_open()) {
        file_.close();
    }
}

void FileSink::write(LogLevel /*level*/, const std::string& message) {
    if (!file_.is_open()) return;

    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    file_ << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
          << '.' << std::setfill('0') << std::setw(3) << ms.count()
          << " " << message << std::endl;
    file_.flush();
}

} // namespace core
