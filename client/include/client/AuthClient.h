#pragma once
#include "Client.h"

class AuthClient {
public:
    explicit AuthClient(Client& c);

    bool registerUser();
    bool loginUser(bool& loggedIn);

private:
    Client& client;
};
