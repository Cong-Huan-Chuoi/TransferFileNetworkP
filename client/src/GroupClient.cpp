#include "client/GroupClient.h"
#include "protocol/packets.h"
#include "protocol/packet_types.h"
#include <iostream>

GroupClient::GroupClient(Client& c) : client(c) {}

void GroupClient::createGroup(const std::string& g) {
    CreateGroupRequest req{g};
    ByteBuffer buf; req.serialize(buf);
    client.sendPacket((uint16_t)PacketType::GROUP_CREATE_REQ, buf);
}

void GroupClient::joinGroup(const std::string& g) {
    JoinGroupRequest req{g};
    ByteBuffer buf; req.serialize(buf);
    client.sendPacket((uint16_t)PacketType::GROUP_JOIN_REQ, buf);
}

void GroupClient::approveJoin(const std::string& g, const std::string& u) {
    ApproveJoinRequest req{g,u};
    ByteBuffer buf; req.serialize(buf);
    client.sendPacket((uint16_t)PacketType::GROUP_APPROVE_REQ, buf);
}

void GroupClient::rejectJoin(const std::string& g, const std::string& u) {
    RejectJoinRequest req{g,u};
    ByteBuffer buf; req.serialize(buf);
    client.sendPacket((uint16_t)PacketType::GROUP_REJECT_JOIN_REQ, buf);
}

void GroupClient::inviteUser(const std::string& g, const std::string& u) {
    InviteUserRequest req{g,u};
    ByteBuffer buf; req.serialize(buf);
    client.sendPacket((uint16_t)PacketType::GROUP_INVITE_REQ, buf);
}

void GroupClient::acceptInvite(const std::string& g) {
    AcceptInviteRequest req{g};
    ByteBuffer buf; req.serialize(buf);
    client.sendPacket((uint16_t)PacketType::GROUP_ACCEPT_INVITE_REQ, buf);
}

void GroupClient::rejectInvite(const std::string& g, const std::string& inviter) {
    RejectInviteRequest req{g,inviter};
    ByteBuffer buf; req.serialize(buf);
    client.sendPacket((uint16_t)PacketType::GROUP_REJECT_INVITE_REQ, buf);
}

void GroupClient::leaveGroup(const std::string& g) {
    LeaveGroupRequest req{g};
    ByteBuffer buf; req.serialize(buf);
    client.sendPacket((uint16_t)PacketType::GROUP_LEAVE_REQ, buf);
}

void GroupClient::kickMember(const std::string& g, const std::string& u) {
    KickMemberRequest req{g,u};
    ByteBuffer buf; req.serialize(buf);
    client.sendPacket((uint16_t)PacketType::GROUP_KICK_REQ, buf);
}

void GroupClient::listMembers(const std::string& g) {
    ListMembersRequest req{g};
    ByteBuffer buf; req.serialize(buf);
    client.sendPacket((uint16_t)PacketType::GROUP_LIST_MEMBERS_REQ, buf);

    PacketHeader hdr; ByteBuffer payload;
    if (client.recvPacket(hdr,payload) && hdr.type==(uint16_t)PacketType::GROUP_LIST_MEMBERS_RES) {
        ListMembersResponse res; res.deserialize(payload);
        std::cout << "Members in group " << g << ":\n";
        for(auto& m:res.members) std::cout << " - " << m << "\n";
    }
}

GroupListCache GroupClient::getGroupLists() {
    ListGroupsRequest req; ByteBuffer buf; req.serialize(buf);
    client.sendPacket((uint16_t)PacketType::GROUP_LIST_REQ, buf);
    PacketHeader hdr; ByteBuffer payload;
    GroupListCache res;
    if (client.recvPacket(hdr,payload) && hdr.type==(uint16_t)PacketType::GROUP_LIST_RES) {
        ListGroupsResponse r; r.deserialize(payload);
        res.owned = r.ownedGroups;
        res.joined = r.joinedGroups;
    }
    return res;
}

void GroupClient::showPending() {
    ListGroupsRequest req; ByteBuffer buf; req.serialize(buf);
    client.sendPacket((uint16_t)PacketType::GROUP_PENDING_REQ, buf);

    PacketHeader hdr; ByteBuffer payload;
    if (client.recvPacket(hdr,payload) && hdr.type==(uint16_t)PacketType::GROUP_PENDING_RES) {
        PendingListResponse res; res.deserialize(payload);
        std::cout << "Pending join requests:\n";
        for(auto& e:res.joinRequests)
            std::cout << " - " << e.username << " wants to join " << e.groupName << "\n";
        std::cout << "Pending invites:\n";
        for(auto& e:res.invites)
            std::cout << " - You are invited to " << e.groupName << " by " << e.username << "\n";
    }
}
