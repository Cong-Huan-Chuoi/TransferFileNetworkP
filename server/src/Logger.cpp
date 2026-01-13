#include "server/Logger.h"
#include <fstream>
#include <ctime>
#include <filesystem>

namespace fs = std::filesystem;

Logger::Logger(const std::string& file)
: logFile(file) {
    try {
        fs::path p = fs::path(logFile).parent_path();
        if (!p.empty() && !fs::exists(p)) {
            fs::create_directories(p);
        }
        // Tạo file nếu chưa có, mở rồi đóng ngay
        std::ofstream out(logFile, std::ios::app);
    } catch (...) {
        // Không ném ngoại lệ ở constructor logger; ghi lỗi có thể được xử lý khác
    }
}

void Logger::log(const std::string& msg) {
    std::lock_guard<std::mutex> lock(mtx);
    std::ofstream out(logFile, std::ios::app);
    if (!out) return;
    std::time_t t = std::time(nullptr);
    // ctime trả về chuỗi có newline, giữ nguyên để dễ đọc
    out << std::ctime(&t) << ": " << msg << "\n";
}
