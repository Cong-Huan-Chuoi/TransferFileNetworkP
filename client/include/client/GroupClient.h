#pragma once
#include <string>

class Client;

class GroupClient {
public:
    explicit GroupClient(Client& c);
    bool handle(const std::string& cmd);

private:
    Client& client;
};
