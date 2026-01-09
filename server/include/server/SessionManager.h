#pragma once
#include <unordered_map>
#include <memory>
#include "ClientSession.h"

class SessionManager {
public:
    void addSession(int fd);
    void removeSession(int fd);
    ClientSession* getSession(int fd);

private:
    std::unordered_map<int, std::unique_ptr<ClientSession>> sessions;
};
