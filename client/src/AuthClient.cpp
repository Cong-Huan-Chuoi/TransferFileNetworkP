#include "client/AuthClient.h"
#include "protocol/packets.h"
#include "protocol/packet_types.h"
#include <iostream>

AuthClient::AuthClient(Client& c) : client(c) {}

bool AuthClient::registerUser() {
    std::string u, p;
    std::cout << "Username: ";
    std::cin >> u;
    std::cout << "Password: ";
    std::cin >> p;

    RegisterRequest req{u, p};
    ByteBuffer buf;
    req.serialize(buf);

    client.sendPacket(
        (uint16_t)PacketType::AUTH_REGISTER_REQ, buf);

    std::cout << "Register success\n";
    return true;
}

bool AuthClient::loginUser(bool& loggedIn) {
    std::string u, p;
    std::cout << "Username: ";
    std::cin >> u;
    std::cout << "Password: ";
    std::cin >> p;

    LoginRequest req{u, p};
    ByteBuffer buf;
    req.serialize(buf);

    client.sendPacket(
        (uint16_t)PacketType::AUTH_LOGIN_REQ, buf);

    loggedIn = true;
    std::cout << "Login success\n";
    return true;
}
