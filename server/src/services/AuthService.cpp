#include "server/services/AuthService.h"
#include "server/AuthManager.h"

AuthService::AuthService(AuthManager& auth)
    : authManager(auth) {}

bool AuthService::registerUser(const std::string& user,
                               const std::string& password) {
    authManager.registerUser(user, password);
    return true;
}

bool AuthService::login(const std::string& user,
                        const std::string& password) {
    return authManager.verifyLogin(user, password)
           == AuthResult::SUCCESS;
}
