#pragma once
#include <string>

class Logger {
public:
    // ===== EXISTING CONTRACT =====
    explicit Logger(const std::string& path);
    void log(const std::string& msg);

    // ===== FOUNDATION ADD (STATIC) =====
    static void info(const std::string& msg);
    static void warn(const std::string& msg);
    static void error(const std::string& msg);

private:
    std::string logPath;
};
