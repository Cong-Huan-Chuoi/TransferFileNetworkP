#include "server/services/AuthService.h"
#include "server/AuthManager.h"

AuthService::AuthService(AuthManager& auth)
    : authManager(auth) {}

// ================= REGISTER =================
bool AuthService::registerUser(const std::string& user,
                               const std::string& password) {
    AuthResult res = authManager.registerUser(user, password);

    return res == AuthResult::SUCCESS;
}

// ================= LOGIN =================
bool AuthService::login(const std::string& user,
                        const std::string& password) {
    AuthResult res = authManager.verifyLogin(user, password);

    return res == AuthResult::SUCCESS;
}
