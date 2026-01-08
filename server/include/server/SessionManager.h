/*
Quản lý các ClientSession
add/remove session khi accept/disconnect
lấy session theo fd
*/

#pragma once
#include <unordered_map>
#include <memory>
#include "server/ClientSession.h"

// Map: fd -> ClientSession

class SessionManager{
    public:
        void add_session(int fd);
        void remove_session(int fd);
        ClientSession* get_session(int fd);

        bool is_logged_in(int fd) const;
    private:
        std::unordered_map<int, std::unique_ptr<ClientSession>> sessions;
};