#include "client/GroupClient.h"
#include "client/Client.h"

GroupClient::GroupClient(Client& c) : client(c) {}

bool GroupClient::handle(const std::string& cmd) {
    if (cmd.starts_with("create_group ")) {
        client.send_packet(PacketType::CREATE_GROUP, cmd.substr(13));
        return true;
    }

    if (cmd == "list_groups") {
        client.send_packet(PacketType::LIST_GROUPS, "");
        return true;
    }

    if (cmd.starts_with("join_group ")) {
        client.send_packet(PacketType::JOIN_GROUP_REQUEST, cmd.substr(11));
        return true;
    }

    return false;
}
