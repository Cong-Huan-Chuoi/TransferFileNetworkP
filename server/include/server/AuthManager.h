#pragma once
#include <string>
#include <unordered_map>

enum class AuthResult {
    SUCCESS,
    USER_EXISTS,
    INVALID_CREDENTIALS
};

class AuthManager {
public:
    explicit AuthManager(const std::string& file);

    AuthResult registerUser(const std::string& user,
                            const std::string& pass);
    AuthResult verifyLogin(const std::string& user,
                           const std::string& pass);

private:
    std::string dbFile;
    std::unordered_map<std::string, std::string> users;

    void saveToFile();
};
