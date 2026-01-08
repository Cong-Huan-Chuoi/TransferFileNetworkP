#include "client/AuthClient.h"
#include "client/Client.h"
#include <iostream>

AuthClient::AuthClient(Client& c) : client(c) {}

bool AuthClient::handle(const std::string& cmd) {
    if (cmd.starts_with("register ")) {
        client.send_packet(PacketType::REGISTER, cmd.substr(9));
        return true;
    }

    if (cmd.starts_with("login ")) {
        client.send_packet(PacketType::LOGIN, cmd.substr(6));
        client.session().logged_in = true;
        return true;
    }

    return false;
}
