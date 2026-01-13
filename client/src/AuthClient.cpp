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

    // ===== ĐỌC RESPONSE =====
    PacketHeader hdr;
    ByteBuffer payload;

    if (!client.recvPacket(hdr, payload)) {
        std::cout << "Server disconnected\n";
        loggedIn = false;
        return false;
    }

    if (hdr.type != (uint16_t)PacketType::AUTH_ACTION_RES) {
        std::cout << "Protocol error\n";
        loggedIn = false;
        return false;
    }

    ActionResultResponse res;
    res.deserialize(payload);

    if (res.ok) {
        loggedIn = true;
        std::cout << res.message << "\n";
        return true;
    } else {
        loggedIn = false;
        std::cout << "Login failed: " << res.message << "\n";
        return false;
    }
}


