#include "server/AuthManager.h"
#include <fstream>
#include <filesystem>

AuthManager::AuthManager(const std::string& file)
    : dbFile(file) {

    std::ifstream in(file);
    std::string line;
    while (std::getline(in, line)) {
        auto pos = line.find('|');
        if (pos == std::string::npos) continue;

        users[line.substr(0, pos)] = line.substr(pos + 1);
    }
}

void AuthManager::saveToFile() {
    std::filesystem::create_directories(
        std::filesystem::path(dbFile).parent_path()
    );

    std::ofstream out(dbFile, std::ios::trunc);
    for (auto& [u, p] : users) {
        out << u << "|" << p << "\n";
    }
}
AuthResult AuthManager::registerUser(const std::string& user,
                                     const std::string& pass) {
    if (users.count(user))
        return AuthResult::USER_EXISTS;

    users[user] = pass;
    saveToFile();              // 🔥 ghi file thật
    return AuthResult::SUCCESS;
}

AuthResult AuthManager::verifyLogin(const std::string& user,
                                    const std::string& pass) {
    if (!users.count(user)) return AuthResult::INVALID_CREDENTIALS;
    if (users[user] != pass) return AuthResult::INVALID_CREDENTIALS;
    return AuthResult::SUCCESS;
}
