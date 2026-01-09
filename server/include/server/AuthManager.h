#pragma once
#include <string>
#include <mutex>

enum class AuthResult {
    SUCCESS,
    USER_EXISTS,
    USER_NOT_FOUND,
    WRONG_PASSWORD,
    FILE_ERROR
};

class AuthManager {
public:
    explicit AuthManager(const std::string& userDbPath);

    AuthResult registerUser(const std::string& username,
                            const std::string& password);

    AuthResult verifyLogin(const std::string& username,
                           const std::string& password);

private:
    std::string dbPath;
    std::mutex dbMutex;

    bool userExists(const std::string& username);
};
