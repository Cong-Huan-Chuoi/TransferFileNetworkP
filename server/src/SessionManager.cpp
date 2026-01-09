#include "server/SessionManager.h"
#include <unistd.h>

void SessionManager::addSession(int fd) {
    auto session = std::make_unique<ClientSession>();
    session->fd = fd;
    sessions[fd] = std::move(session);
}

void SessionManager::removeSession(int fd) {
    sessions.erase(fd);
    close(fd);
}

ClientSession* SessionManager::getSession(int fd) {
    auto it = sessions.find(fd);
    if (it == sessions.end()) return nullptr;
    return it->second.get();
}
