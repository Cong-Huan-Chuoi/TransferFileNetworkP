#pragma once
#include <string>

class Client;

class AuthClient {
public:
    explicit AuthClient(Client& c);
    bool handle(const std::string& cmd);

private:
    Client& client;
};
