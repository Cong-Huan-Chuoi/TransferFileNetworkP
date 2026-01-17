#include "server/DataPaths.h"
#include <filesystem>

namespace fs = std::filesystem;

namespace DataPaths {

std::string projectRoot() {
    // giả định server chạy từ build/
    // => project root = parent of build
    fs::path cwd = fs::current_path();

    if (cwd.filename() == "build") {
        return cwd.parent_path().string();
    }
    return cwd.string();
}

std::string dataDir() {
    return (fs::path(projectRoot()) / "data").string();
}

std::string usersDb() {
    return (fs::path(dataDir()) / "users.db").string();
}

std::string groupsDb() {
    return (fs::path(dataDir()) / "groups.db").string();
}

std::string groupsDir() {
    return (fs::path(dataDir()) / "groups").string();
}

}
