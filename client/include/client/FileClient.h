#pragma once
#include "Client.h"

class FileClient {
public:
    explicit FileClient(Client& c);

    void listFiles();

private:
    Client& client;
};
