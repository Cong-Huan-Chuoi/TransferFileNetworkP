#pragma once
#include <string>

class Client;

class FileClient {
public:
    explicit FileClient(Client& c);
    bool handle(const std::string& cmd);

private:
    void upload(const std::string& path);
    void download(const std::string& path);

    Client& client;
};
