#include "protocol/packets.h"
#include <cstdint>
#include <vector>

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

void AuthLoginResponse::serialize(ByteBuffer& buf) const {
    buf.writeBool(success);
    buf.writeString(message);
}

void AuthLoginResponse::deserialize(ByteBuffer& buf) {
    success = buf.readBool();
    message = buf.readString();
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
    for (uint32_t i = 0; i < n; ++i)
        members.push_back(buf.readString());
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
    for (uint32_t i = 0; i < n1; ++i)
        ownedGroups.push_back(buf.readString());

    uint32_t n2 = buf.read<uint32_t>();
    joinedGroups.clear();
    for (uint32_t i = 0; i < n2; ++i)
        joinedGroups.push_back(buf.readString());
}

void PendingEntry::serialize(ByteBuffer& buf) const {
    buf.writeString(groupName);
    buf.writeString(username);
    buf.writeString(inviter);
}
void PendingEntry::deserialize(ByteBuffer& buf) {
    groupName = buf.readString();
    username = buf.readString();
    inviter = buf.readString();
}

void PendingListResponse::serialize(ByteBuffer& buf) const {
    buf.write<uint32_t>(joinRequests.size());
    for (auto& e : joinRequests) e.serialize(buf);
    buf.write<uint32_t>(invites.size());
    for (auto& e : invites) e.serialize(buf);
}
void PendingListResponse::deserialize(ByteBuffer& buf) {
    uint32_t n1 = buf.read<uint32_t>();
    joinRequests.clear();
    for (uint32_t i = 0; i < n1; ++i) {
        PendingEntry e; e.deserialize(buf);
        joinRequests.push_back(e);
    }
    uint32_t n2 = buf.read<uint32_t>();
    invites.clear();
    for (uint32_t i = 0; i < n2; ++i) {
        PendingEntry e; e.deserialize(buf);
        invites.push_back(e);
    }
}

void RejectJoinRequest::serialize(ByteBuffer& buf) const {
    buf.writeString(groupName);
    buf.writeString(username);
}
void RejectJoinRequest::deserialize(ByteBuffer& buf) {
    groupName = buf.readString();
    username = buf.readString();
}

void RejectInviteRequest::serialize(ByteBuffer& buf) const {
    buf.writeString(groupName);
    buf.writeString(username);
}
void RejectInviteRequest::deserialize(ByteBuffer& buf) {
    groupName = buf.readString();
    username = buf.readString();
}

// ===== FILE CONTROL =====
void UploadFileRequest::serialize(ByteBuffer& buf) const {
    buf.writeString(groupName);
    buf.writeString(remotePath);
    buf.write<uint64_t>(fileSize);   
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

void FileListRequest::serialize(ByteBuffer& buf) const {
    buf.writeString(groupName);
    buf.writeString(path);
}
void FileListRequest::deserialize(ByteBuffer& buf) {
    groupName = buf.readString();
    path = buf.readString();
}

// ===== FILE LIST RESPONSE (CORE) =====
void FileListResponse::serialize(ByteBuffer& buf) const {
    buf.write<uint32_t>(entries.size());
    for (const auto& e : entries) {
        buf.writeString(e.name);
        buf.write<uint8_t>(e.isDir ? 1 : 0);
        buf.write<uint64_t>(e.size);
    }
}
void FileListResponse::deserialize(ByteBuffer& buf) {
    uint32_t n = buf.read<uint32_t>();
    entries.clear();
    entries.reserve(n);
    for (uint32_t i = 0; i < n; ++i) {
        FileListEntry e;
        e.name = buf.readString();
        e.isDir = buf.read<uint8_t>() != 0;
        e.size = buf.read<uint64_t>();
        entries.push_back(e);
    }
}

void MkdirRequest::serialize(ByteBuffer& buf) const {
    buf.writeString(groupName);
    buf.writeString(path);
}
void MkdirRequest::deserialize(ByteBuffer& buf) {
    groupName = buf.readString();
    path = buf.readString();
}

void DeleteRequest::serialize(ByteBuffer& buf) const {
    buf.writeString(groupName);
    buf.writeString(path);
}
void DeleteRequest::deserialize(ByteBuffer& buf) {
    groupName = buf.readString();
    path = buf.readString();
}

void RenameRequest::serialize(ByteBuffer& buf) const {
    buf.writeString(groupName);
    buf.writeString(oldPath);
    buf.writeString(newPath);
}
void RenameRequest::deserialize(ByteBuffer& buf) {
    groupName = buf.readString();
    oldPath = buf.readString();
    newPath = buf.readString();
}

void CopyRequest::serialize(ByteBuffer& buf) const {
    buf.writeString(groupName);
    buf.writeString(srcPath);
    buf.writeString(dstPath);
}
void CopyRequest::deserialize(ByteBuffer& buf) {
    groupName = buf.readString();
    srcPath = buf.readString();
    dstPath = buf.readString();
}

void MoveRequest::serialize(ByteBuffer& buf) const {
    buf.writeString(groupName);
    buf.writeString(srcPath);
    buf.writeString(dstPath);
}
void MoveRequest::deserialize(ByteBuffer& buf) {
    groupName = buf.readString();
    srcPath = buf.readString();
    dstPath = buf.readString();
}

void ActionResult::serialize(ByteBuffer& buf) const {
    buf.write<uint8_t>(success ? 1 : 0);
    buf.writeString(message);
}
void ActionResult::deserialize(ByteBuffer& buf) {
    success = buf.read<uint8_t>() != 0;
    message = buf.readString();
}

// ===== FILE TRANSFER =====
void UploadBegin::serialize(ByteBuffer& buf) const {
    buf.writeString(groupName);
    buf.writeString(remotePath);
    buf.write<uint64_t>(totalSize);
}
void UploadBegin::deserialize(ByteBuffer& buf) {
    groupName = buf.readString();
    remotePath = buf.readString();
    totalSize = buf.read<uint64_t>();
}

void UploadChunk::serialize(ByteBuffer& buf) const {
    buf.write<uint32_t>(data.size());
    if (!data.empty())
        buf.append(reinterpret_cast<const char*>(data.data()), data.size());
}
void UploadChunk::deserialize(ByteBuffer& buf) {
    uint32_t n = buf.read<uint32_t>();
    data.resize(n);
    for (uint32_t i = 0; i < n; ++i)
        data[i] = buf.read<uint8_t>();
}

void DownloadBegin::serialize(ByteBuffer& buf) const {
    buf.writeString(groupName);
    buf.writeString(remotePath);
}
void DownloadBegin::deserialize(ByteBuffer& buf) {
    groupName = buf.readString();
    remotePath = buf.readString();
}

void DownloadChunk::serialize(ByteBuffer& buf) const {
    buf.write<uint32_t>(data.size());
    if (!data.empty())
        buf.append(reinterpret_cast<const char*>(data.data()), data.size());
}
void DownloadChunk::deserialize(ByteBuffer& buf) {
    uint32_t n = buf.read<uint32_t>();
    data.resize(n);
    for (uint32_t i = 0; i < n; ++i)
        data[i] = buf.read<uint8_t>();
}
