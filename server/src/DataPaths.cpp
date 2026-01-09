#include "server/DataPaths.h"
#include <filesystem>
#include <stdexcept>

namespace fs = std::filesystem;

// ===== STATIC STORAGE =====
fs::path DataPaths::projectRoot;

// ===== INIT (CALL ONCE AT STARTUP) =====
void DataPaths::init(const fs::path& executablePath) {
    // executablePath = argv[0]
    fs::path p = fs::absolute(executablePath).parent_path();

    // đi ngược lên cho tới khi thấy CMakeLists.txt
    while (!p.empty()) {
        if (fs::exists(p / "CMakeLists.txt")) {
            projectRoot = p;
            return;
        }
        p = p.parent_path();
    }

    throw std::runtime_error("Cannot locate project root (CMakeLists.txt)");
}

// ===== EXISTING CONTRACT =====
std::string DataPaths::groupsDir() {
    return (dataRoot() / "groups").string();
}

std::string DataPaths::usersDb() {
    return (dataRoot() / "users.db").string();
}

std::string DataPaths::groupsDb() {
    return (dataRoot() / "groups.db").string();
}

// ===== CORE IMPLEMENTATION =====
fs::path DataPaths::dataRoot() {
    if (projectRoot.empty()) {
        throw std::runtime_error("DataPaths::init() not called");
    }
    return ensureDir(projectRoot / "data");
}

fs::path DataPaths::groupRoot(const std::string& groupId) {
    if (groupId.empty())
        throw std::runtime_error("groupId empty");
    return ensureDir(dataRoot() / "groups" / groupId);
}

fs::path DataPaths::groupFiles(const std::string& groupId) {
    return ensureDir(groupRoot(groupId) / "files");
}

fs::path DataPaths::ensureDir(const fs::path& p) {
    if (!fs::exists(p)) {
        fs::create_directories(p);
    }
    return p;
}
