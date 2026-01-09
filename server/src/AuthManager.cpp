#include "server/AuthManager.h"
#include <fstream>
#include <sstream>

AuthManager::AuthManager(const std::string& userDbPath)
    : dbPath(userDbPath) {}

bool AuthManager::userExists(const std::string& username) {
    std::ifstream in(dbPath);
    if (!in.is_open()) return false;

    std::string line;
    while (std::getline(in, line)) {
        std::istringstream iss(line);
        std::string user;
        if (std::getline(iss, user, '|')) {
            if (user == username) return true;
        }
    }
    return false;
}

AuthResult AuthManager::registerUser(const std::string& username,
                                     const std::string& password) {
    std::lock_guard<std::mutex> lock(dbMutex);

    if (userExists(username))
        return AuthResult::USER_EXISTS;

    std::ofstream out(dbPath, std::ios::app);
    if (!out.is_open())
        return AuthResult::FILE_ERROR;

    out << username << "|" << password << "\n";
    return AuthResult::SUCCESS;
}

AuthResult AuthManager::verifyLogin(const std::string& username,
                                    const std::string& password) {
    std::lock_guard<std::mutex> lock(dbMutex);

    std::ifstream in(dbPath);
    if (!in.is_open())
        return AuthResult::FILE_ERROR;

    std::string line;
    while (std::getline(in, line)) {
        std::istringstream iss(line);
        std::string user, pass;
        if (std::getline(iss, user, '|') &&
            std::getline(iss, pass)) {
            if (user == username) {
                if (pass == password)
                    return AuthResult::SUCCESS;
                else
                    return AuthResult::WRONG_PASSWORD;
            }
        }
    }
    return AuthResult::USER_NOT_FOUND;
}
