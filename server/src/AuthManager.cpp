#include "server/AuthManager.h"
#include <fstream>
#include <sstream>

// khởi tạo AuthManager: load user.db vào memory
AuthManager::AuthManager(const std::string& user_db_path)
    : db_path(user_db_path){
        load_user();
}

// đọc file users.db
void AuthManager::load_user(){
    std::ifstream file(db_path);
    if(!file.is_open())
        return;
    
    std::string line;
    while (std::getline(file, line)){
        std::istringstream iss(line);
        std::string username, password;
        if (std::getline(iss, username, ':') &&
            std::getline(iss, password)){
                users[username] = password;
            }
    }
}

// ghi thêm user mới vào users.db

void AuthManager::save_user(const std::string& username,
                            const std::string& password){
    std::ofstream file(db_path, std::ios::app);
    file << username << ":" << password << "\n";
}

// register

bool AuthManager::register_user(const std::string& username,
                                const std::string& password) {
    if (users.count(username))
        return false;

    users[username] = password;
    save_user(username, password);
    return true;
}

/*
 * Đăng nhập
 */
bool AuthManager::login_user(int fd,
                             const std::string& username,
                             const std::string& password,
                             SessionManager& sessionManager) {
    auto it = users.find(username);
    if (it == users.end())
        return false;

    if (it->second != password)
        return false;

    ClientSession* session = sessionManager.get_session(fd);
    if (!session)
        return false;

    session->logged_in = true;
    session->username = username;
    return true;
}

/*
 * Đăng xuất
 */
void AuthManager::logout_user(int fd,
                              SessionManager& sessionManager) {
    ClientSession* session = sessionManager.get_session(fd);
    if (!session)
        return;

    session->logged_in = false;
    session->username.clear();
    session->current_group.clear();
}