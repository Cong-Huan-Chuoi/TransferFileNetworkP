#include "server/Logger.h"
#include <fstream>
#include <ctime>

Logger::Logger(const std::string& file)
    : logFile(file) {}

void Logger::log(const std::string& msg) {
    std::lock_guard<std::mutex> lock(mtx);

    std::ofstream out(logFile, std::ios::app);
    std::time_t t = std::time(nullptr);
    out << std::ctime(&t) << ": " << msg << "\n";
}
