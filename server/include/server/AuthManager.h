#pragma once
#include<string>
#include<unordered_map>
#include "server/SessionManager.h"

/*
 * AuthManager
   - Quản lý đăng ký / đăng nhập
   - Làm việc với SessionManager
 */

 class AuthManager{
    public:
        explicit AuthManager(const std::string& user_db_path);
        bool register_user(const std::string& username, const std::string& password);
        bool login_user(int fd,
                    const std::string& username,
                    const std::string& password,
                    SessionManager& sessionManager);

        void logout_user(int fd, SessionManager& sessionManager);
    private:
        void load_user();
        void save_user(const std::string& username,
                       const std::string& password);
    private:
        std::string db_path;
        std::unordered_map<std::string, std::string> users; //username->pass         
 };