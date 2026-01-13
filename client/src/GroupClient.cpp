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
    client.sendPacket((uint16_t)PacketType::GROUP_CREATE_REQ, buf);
}

void GroupClient::joinGroup() {
    std::string g;
    std::cout << "Group name: ";
    std::cin >> g;

    JoinGroupRequest req{g};
    ByteBuffer buf;
    req.serialize(buf);
    client.sendPacket((uint16_t)PacketType::GROUP_JOIN_REQ, buf);
}

void GroupClient::approveJoin() {
    std::string g, u;
    std::cout << "Group name: "; std::cin >> g;
    std::cout << "Username to approve: "; std::cin >> u;

    ApproveJoinRequest req{g, u};
    ByteBuffer buf; req.serialize(buf);
    client.sendPacket((uint16_t)PacketType::GROUP_APPROVE_REQ, buf);
}

void GroupClient::inviteUser() {
    std::string g, u;
    std::cout << "Group name: "; std::cin >> g;
    std::cout << "Username to invite: "; std::cin >> u;

    InviteUserRequest req{g, u};
    ByteBuffer buf; req.serialize(buf);
    client.sendPacket((uint16_t)PacketType::GROUP_INVITE_REQ, buf);
}

void GroupClient::acceptInvite() {
    std::string g;
    std::cout << "Group name to accept invite: ";
    std::cin >> g;

    AcceptInviteRequest req{g};
    ByteBuffer buf; req.serialize(buf);
    client.sendPacket((uint16_t)PacketType::GROUP_ACCEPT_INVITE_REQ, buf);
}

void GroupClient::leaveGroup() {
    std::string g;
    std::cout << "Group name to leave: ";
    std::cin >> g;

    LeaveGroupRequest req{g};
    ByteBuffer buf; req.serialize(buf);
    client.sendPacket((uint16_t)PacketType::GROUP_LEAVE_REQ, buf);
}

void GroupClient::kickMember() {
    std::string g, u;
    std::cout << "Group name: "; std::cin >> g;
    std::cout << "Username to kick: "; std::cin >> u;

    KickMemberRequest req{g, u};
    ByteBuffer buf; req.serialize(buf);
    client.sendPacket((uint16_t)PacketType::GROUP_KICK_REQ, buf);
}

void GroupClient::listMembers() {
    std::string g;
    std::cout << "Group name: "; std::cin >> g;

    ListMembersRequest req{g};
    ByteBuffer buf; req.serialize(buf);
    client.sendPacket((uint16_t)PacketType::GROUP_LIST_MEMBERS_REQ, buf);

    PacketHeader hdr;
    ByteBuffer payload;
    if (client.recvPacket(hdr, payload) && hdr.type == (uint16_t)PacketType::GROUP_LIST_MEMBERS_RES) {
        ListMembersResponse res;
        res.deserialize(payload);
        std::cout << "Members in group:\n";
        for (auto& m : res.members) std::cout << " - " << m << "\n";
    } else {
        std::cout << "Failed to receive member list.\n";
    }
}

GroupListCache GroupClient::getGroupLists() {
    ListGroupsRequest req;
    ByteBuffer buf;
    req.serialize(buf);

    // DEBUG: announce request from UI
    std::cout << "[Client UI] Requesting group lists for user\n";

    if (!client.sendPacket((uint16_t)PacketType::GROUP_LIST_REQ, buf)) {
        std::cout << "[Client] send GROUP_LIST_REQ failed\n";
        return {};
    }
    std::cout << "[Client] GROUP_LIST_REQ sent\n";

    PacketHeader hdr;
    ByteBuffer payload;
    GroupListCache res;

    if (!client.recvPacket(hdr, payload)) {
        std::cout << "[Client] recvPacket failed (no response)\n";
        return res;
    }

    std::cout << "[Client] recvPacket got type=" << hdr.type << " length=" << hdr.length << "\n";
    if (hdr.type != (uint16_t)PacketType::GROUP_LIST_RES) {
        std::cout << "[Client] unexpected packet type: " << hdr.type << "\n";
        return res;
    }

    try {
        ListGroupsResponse r;
        r.deserialize(payload);
        res.owned = r.ownedGroups;
        res.joined = r.joinedGroups;
        std::cout << "[Client] Owned groups: " << res.owned.size()
                  << ", Joined groups: " << res.joined.size() << "\n";
    } catch (...) {
        std::cout << "[Client] failed to parse GROUP_LIST_RES payload\n";
    }

    return res;
}
