#include "server/Logger.h"
#include <chrono>
#include <ctime>

Logger::Logger(const std::string& file) {
    ofs.open(file, std::ios::app);
}

void Logger::log(LogLevel level, const std::string& msg) {
    std::lock_guard<std::mutex> lock(mtx);

    auto now = std::chrono::system_clock::to_time_t(
        std::chrono::system_clock::now());

    ofs << std::ctime(&now)
        << "[" << level_to_string(level) << "] "
        << msg << "\n";
}

void Logger::info(const std::string& msg) {
    log(LogLevel::INFO, msg);
}

void Logger::warn(const std::string& msg) {
    log(LogLevel::WARN, msg);
}

void Logger::error(const std::string& msg) {
    log(LogLevel::ERROR, msg);
}

std::string Logger::level_to_string(LogLevel level) {
    switch (level) {
        case LogLevel::INFO:  return "INFO";
        case LogLevel::WARN:  return "WARN";
        case LogLevel::ERROR: return "ERROR";
    }
    return "UNKNOWN";
}
