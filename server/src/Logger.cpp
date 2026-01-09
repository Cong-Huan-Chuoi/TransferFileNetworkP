#include "server/Logger.h"
#include <fstream>
#include <iostream>
#include <ctime>

static std::string now() {
    std::time_t t = std::time(nullptr);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%F %T", std::localtime(&t));
    return buf;
}

// ===== EXISTING IMPLEMENTATION =====
Logger::Logger(const std::string& path) : logPath(path) {}

void Logger::log(const std::string& msg) {
    std::ofstream ofs(logPath, std::ios::app);
    ofs << "[" << now() << "] " << msg << "\n";
}

// ===== FOUNDATION STATIC LOG =====
void Logger::info(const std::string& msg) {
    std::cout << "[INFO][" << now() << "] " << msg << "\n";
}

void Logger::warn(const std::string& msg) {
    std::cout << "[WARN][" << now() << "] " << msg << "\n";
}

void Logger::error(const std::string& msg) {
    std::cerr << "[ERROR][" << now() << "] " << msg << "\n";
}
