#pragma once
#include "Client.h"

class GroupClient {
public:
    explicit GroupClient(Client& c);

    void createGroup();
    void joinGroup();

private:
    Client& client;
};
