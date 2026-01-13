#pragma once
#include <string>

class AuthManager;

class AuthService {
public:
    explicit AuthService(AuthManager& auth);

    bool registerUser(const std::string& user,
                      const std::string& password);

    bool login(const std::string& user,
               const std::string& password);

private:
    AuthManager& authManager;
};
