#include "client/GroupClient.h"
#include "protocol/packets.h"
#include "protocol/packet_types.h"
#include <iostream>

GroupClient::GroupClient(Client& c) : client(c) {}

static void recvAndPrintResult(Client& client) {
    PacketHeader hdr;
    ByteBuffer payload;

    if (!client.recvPacket(hdr, payload)) return;

    if (hdr.type == (uint16_t)PacketType::GROUP_ACTION_RES) {
        ActionResultResponse res;
        res.deserialize(payload);

        if (res.ok)
            std::cout << "[OK] " << res.message << "\n";
        else
            std::cout << "[ERROR] " << res.message << "\n";
    }
}

void GroupClient::createGroup() {
    std::string g;
    std::cout << "Group name: ";
    std::cin >> g;

    CreateGroupRequest req{g};
    ByteBuffer buf;
    req.serialize(buf);

    client.sendPacket(
        (uint16_t)PacketType::GROUP_CREATE_REQ, buf);

    recvAndPrintResult(client);
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

    recvAndPrintResult(client);
}   

void GroupClient::approveJoin() {
    std::string g,u;
    std::cout << "Group name: "; std::cin >> g;
    std::cout << "Username to approve: "; std::cin >> u;

    ApproveJoinRequest req{g,u};
    ByteBuffer buf;
    req.serialize(buf);

    client.sendPacket(
        (uint16_t)PacketType::GROUP_APPROVE_REQ, buf);

    recvAndPrintResult(client);
}

void GroupClient::rejectJoin() {
    std::string g, u;
    std::cout << "Group name: ";
    std::cin >> g;
    std::cout << "Username to reject: ";
    std::cin >> u;

    RejectJoinRequest req{g, u};
    ByteBuffer buf;
    req.serialize(buf);

    client.sendPacket(
        (uint16_t)PacketType::GROUP_REJECT_JOIN_REQ, buf);

    recvAndPrintResult(client);
}

void GroupClient::acceptInvite() {
    std::string g;
    std::cout << "Group name: ";
    std::cin >> g;

    AcceptInviteRequest req{g};
    ByteBuffer buf;
    req.serialize(buf);

    client.sendPacket(
        (uint16_t)PacketType::GROUP_ACCEPT_INVITE_REQ, buf);

    recvAndPrintResult(client);
}

void GroupClient::leaveGroup() {
    std::string g;
    std::cout << "Group name: ";
    std::cin >> g;

    LeaveGroupRequest req{g};
    ByteBuffer buf;
    req.serialize(buf);

    client.sendPacket(
        (uint16_t)PacketType::GROUP_LEAVE_REQ, buf);

    recvAndPrintResult(client);
}

void GroupClient::kickMember() {
    std::string g,u;
    std::cout << "Group name: "; std::cin >> g;
    std::cout << "Username to kick: "; std::cin >> u;

    KickMemberRequest req{g,u};
    ByteBuffer buf;
    req.serialize(buf);

    client.sendPacket(
        (uint16_t)PacketType::GROUP_KICK_REQ, buf);

    recvAndPrintResult(client);
}

void GroupClient::listGroups() {
    ListGroupsRequest req;
    ByteBuffer buf;
    req.serialize(buf);
    client.sendPacket((uint16_t)PacketType::GROUP_LIST_REQ, buf);

    PacketHeader hdr;
    ByteBuffer payload;
    if (client.recvPacket(hdr, payload)) {
        if (hdr.type == (uint16_t)PacketType::GROUP_LIST_RES) {
            ListGroupsResponse res;
            res.deserialize(payload);

            std::cout << "Groups you own:\n";
            for (auto& g : res.ownedGroups) {
                std::cout << " - " << g << "\n";
            }

            std::cout << "Groups you joined:\n";
            for (auto& g : res.joinedGroups) {
                std::cout << " - " << g << "\n";
            }

            if (res.ownedGroups.empty() && res.joinedGroups.empty()) {
            std::cout << "(No groups)\n";
            }
        }
    }
}

