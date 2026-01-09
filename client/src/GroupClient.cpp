#include "client/GroupClient.h"
#include "protocol/packets.h"
#include "protocol/packet_types.h"
#include <iostream>

GroupClient::GroupClient(Client& c) : client(c) {}

void GroupClient::createGroup() {
    std::string g;
    std::cout << "Group name: ";
    std::cin >> g;

    CreateGroupRequest req{g};
    ByteBuffer buf;
    req.serialize(buf);

    client.sendPacket(
        (uint16_t)PacketType::GROUP_CREATE_REQ, buf);
}

void GroupClient::joinGroup() {
    std::string g;
    std::cout << "Group name: ";
    std::cin >> g;

    JoinGroupRequest req{g};
    ByteBuffer buf;
    req.serialize(buf);

    client.sendPacket(
        (uint16_t)PacketType::GROUP_JOIN_REQ, buf);
}
