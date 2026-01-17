#pragma once
#include <string>
#include <mutex>

class Logger {
public:
    explicit Logger(const std::string& file);
    void log(const std::string& msg);

private:
    std::string logFile;
    std::mutex mtx;
};
