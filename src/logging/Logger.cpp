#include "Logger.h"

Logger::Logger() {
    m_worker = std::thread(&Logger::workerLoop, this);
}

Logger::~Logger() {
    m_running = false;
    m_cv.notify_one();
    if (m_worker.joinable()) {
        m_worker.join();
    }
}

Logger& Logger::instance() {
    static Logger inst;
    return inst;
}

void Logger::setFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_hasFile) m_file.close();
    m_file.open(filepath, std::ios::app);
    m_hasFile = m_file.is_open();
}

void Logger::setMinLevel(LogLevel level) {
    m_minLevel = level;
}

void Logger::log(LogLevel level, const std::string& tag, const std::string& msg) {
    if (level < m_minLevel) return;
    std::string line = timestamp() + " [" + levelStr(level) + "] [" + tag + "] " + msg;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(std::move(line));
    }
    m_cv.notify_one();
}

void Logger::workerLoop() {
    while (m_running) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.wait(lock, [this] { return !m_queue.empty() || !m_running; });

        // 排空队列后再退出
        while (!m_queue.empty()) {
            std::string line = std::move(m_queue.front());
            m_queue.pop();
            lock.unlock();
            writeLine(line);
            lock.lock();
        }
    }
}

void Logger::writeLine(const std::string& line) {
    std::cout << line << std::endl;
    // m_file 由 setFile 加锁保护，但 writeLine 只在 worker 线程调用
    if (m_hasFile && m_file.is_open()) {
        m_file << line << std::endl;
    }
}

std::string Logger::levelStr(LogLevel level) {
    switch (level) {
        case LogLevel::Debug:   return "DEBUG";
        case LogLevel::Info:    return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error:   return "ERROR";
    }
    return "???";
}

std::string Logger::timestamp() {
    auto t = std::time(nullptr);
    std::tm tm;
#ifdef _MSC_VER
    localtime_s(&tm, &t);
#else
    tm = *std::localtime(&t);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}
