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

    if (!client.sendPacket((uint16_t)PacketType::AUTH_REGISTER_REQ, buf)) {
        std::cout << "Send failed\n";
        return false;
    }

    PacketHeader hdr; ByteBuffer payload;
    if (!client.recvPacket(hdr, payload) || hdr.type != (uint16_t)PacketType::AUTH_REGISTER_RES) {
        std::cout << "No register response from server\n";
        return false;
    }

    // DEBUG
    std::cout << "[Client] recvPacket: type=" << hdr.type << " length=" << hdr.length << "\n";
    if (hdr.type != (uint16_t)PacketType::AUTH_REGISTER_RES) {
        std::cout << "Unexpected packet type for register: " << hdr.type << "\n";
        return false;
    }

    // Parse ActionResult trước, fallback sang AuthLoginResponse
    try {
        ActionResult ar;
        ar.deserialize(payload);
        std::cout << (ar.success ? "Register success: " : "Register failed: ") << ar.message << "\n";
        return ar.success;
    } catch (...) {
        try {
            AuthLoginResponse res;
            res.deserialize(payload);
            std::cout << (res.success ? "Register success: " : "Register failed: ") << res.message << "\n";
            return res.success;
        } catch (...) {
            std::cout << "Unknown register response\n";
            return false;
        }
    }
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

    if (!client.sendPacket((uint16_t)PacketType::AUTH_LOGIN_REQ, buf)) {
        std::cout << "Send failed\n";
        loggedIn = false;
        return false;
    }

    PacketHeader hdr; ByteBuffer payload;
    if (!client.recvPacket(hdr, payload) || hdr.type != (uint16_t)PacketType::AUTH_LOGIN_RES) {
        std::cout << "No login response from server\n";
        loggedIn = false;
        return false;
    }

    // DEBUG
    std::cout << "[Client] recvPacket: type=" << hdr.type << " length=" << hdr.length << "\n";
    if (hdr.type != (uint16_t)PacketType::AUTH_LOGIN_RES) {
        std::cout << "Unexpected packet type for login: " << hdr.type << "\n";
        loggedIn = false;
        return false;
    }

    AuthLoginResponse res;
    try {
        res.deserialize(payload);
    } catch (...) {
        std::cout << "Malformed login response\n";
        loggedIn = false;
        return false;
    }

    loggedIn = res.success;
    std::cout << (res.success ? "Login success\n" : ("Login failed: " + res.message + "\n"));
    return res.success;
}
