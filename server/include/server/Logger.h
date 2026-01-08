#pragma once

#include <string>
#include <fstream>
#include <mutex>

enum class LogLevel {
    INFO,
    WARN,
    ERROR
};

/*
 * Logger
 *  - Ghi log server
 */
class Logger {
public:
    explicit Logger(const std::string& file);

    void log(LogLevel level, const std::string& msg);

    void info(const std::string& msg);
    void warn(const std::string& msg);
    void error(const std::string& msg);

private:
    std::ofstream ofs;
    std::mutex mtx;

    std::string level_to_string(LogLevel level);
};
