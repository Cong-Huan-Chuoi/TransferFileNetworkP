#include "protocol/packets.h"

// ===== AUTH =====
void RegisterRequest::serialize(ByteBuffer& buf) const {
    buf.writeString(username);
    buf.writeString(password);
}

void RegisterRequest::deserialize(ByteBuffer& buf) {
    username = buf.readString();
    password = buf.readString();
}

void LoginRequest::serialize(ByteBuffer& buf) const {
    buf.writeString(username);
    buf.writeString(password);
}

void LoginRequest::deserialize(ByteBuffer& buf) {
    username = buf.readString();
    password = buf.readString();
}

// ===== GROUP =====
void CreateGroupRequest::serialize(ByteBuffer& buf) const {
    buf.writeString(groupName);
}

void CreateGroupRequest::deserialize(ByteBuffer& buf) {
    groupName = buf.readString();
}

void JoinGroupRequest::serialize(ByteBuffer& buf) const {
    buf.writeString(groupName);
}

void JoinGroupRequest::deserialize(ByteBuffer& buf) {
    groupName = buf.readString();
}

void ApproveJoinRequest::serialize(ByteBuffer& buf) const {
    buf.writeString(groupName);
    buf.writeString(username);
}
void ApproveJoinRequest::deserialize(ByteBuffer& buf) {
    groupName = buf.readString();
    username = buf.readString();
}

void InviteUserRequest::serialize(ByteBuffer& buf) const {
    buf.writeString(groupName);
    buf.writeString(username);
}
void InviteUserRequest::deserialize(ByteBuffer& buf) {
    groupName = buf.readString();
    username = buf.readString();
}

void AcceptInviteRequest::serialize(ByteBuffer& buf) const {
    buf.writeString(groupName);
}
void AcceptInviteRequest::deserialize(ByteBuffer& buf) {
    groupName = buf.readString();
}

void LeaveGroupRequest::serialize(ByteBuffer& buf) const {
    buf.writeString(groupName);
}
void LeaveGroupRequest::deserialize(ByteBuffer& buf) {
    groupName = buf.readString();
}

void KickMemberRequest::serialize(ByteBuffer& buf) const {
    buf.writeString(groupName);
    buf.writeString(username);
}
void KickMemberRequest::deserialize(ByteBuffer& buf) {
    groupName = buf.readString();
    username = buf.readString();
}

void ListMembersRequest::serialize(ByteBuffer& buf) const {
    buf.writeString(groupName);
}
void ListMembersRequest::deserialize(ByteBuffer& buf) {
    groupName = buf.readString();
}

void ListMembersResponse::serialize(ByteBuffer& buf) const {
    buf.write<uint32_t>(members.size());
    for (auto& m : members) buf.writeString(m);
}
void ListMembersResponse::deserialize(ByteBuffer& buf) {
    uint32_t n = buf.read<uint32_t>();
    members.clear();
    for (uint32_t i = 0; i < n; ++i) members.push_back(buf.readString());
}

void ListGroupsResponse::serialize(ByteBuffer& buf) const {
    buf.write<uint32_t>(ownedGroups.size());
    for (auto& g : ownedGroups) buf.writeString(g);

    buf.write<uint32_t>(joinedGroups.size());
    for (auto& g : joinedGroups) buf.writeString(g);
}

void ListGroupsResponse::deserialize(ByteBuffer& buf) {
    uint32_t n1 = buf.read<uint32_t>();
    ownedGroups.clear();
    for (uint32_t i = 0; i < n1; ++i) ownedGroups.push_back(buf.readString());

    uint32_t n2 = buf.read<uint32_t>();
    joinedGroups.clear();
    for (uint32_t i = 0; i < n2; ++i) joinedGroups.push_back(buf.readString());
}



// ===== FILE CONTROL =====
void UploadFileRequest::serialize(ByteBuffer& buf) const {
    buf.writeString(groupName);
    buf.writeString(remotePath);
    buf.write(fileSize);
}

void UploadFileRequest::deserialize(ByteBuffer& buf) {
    groupName = buf.readString();
    remotePath = buf.readString();
    fileSize = buf.read<uint64_t>();
}

void DownloadFileRequest::serialize(ByteBuffer& buf) const {
    buf.writeString(groupName);
    buf.writeString(remotePath);
}

void DownloadFileRequest::deserialize(ByteBuffer& buf) {
    groupName = buf.readString();
    remotePath = buf.readString();
}
