#include "server/SessionManager.h"
#include <unistd.h>

void SessionManager::add_session(int fd){
    sessions.emplace(fd, std::make_unique<ClientSession>(fd));
}

void SessionManager::remove_session(int fd){
    auto it = sessions.find(fd);
    if (it != sessions.end()){
        close(fd);
        sessions.erase(it);
    }
}

ClientSession* SessionManager::get_session(int fd){
    auto it = sessions.find(fd);
    if (it == sessions.end())
        return nullptr;
    return it->second.get();
}
bool SessionManager::is_logged_in(int fd) const {
    auto it = sessions.find(fd);
    if (it == sessions.end())
        return false;
    return it->second->logged_in;
}
