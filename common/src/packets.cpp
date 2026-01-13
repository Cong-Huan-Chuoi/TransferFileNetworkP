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

// Serialize: ghi dữ liệu vào buffer
void AuthLoginResponse::serialize(ByteBuffer& buf) const {
    buf.writeBool(success);
    buf.writeString(message);
}

// Deserialize: đọc dữ liệu từ buffer
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

// packets.cpp (bổ sung phần FILE CONTROL)
void FileListRequest::serialize(ByteBuffer& buf) const {
  buf.writeString(groupName);
  buf.writeString(path);
}
void FileListRequest::deserialize(ByteBuffer& buf) {
  groupName = buf.readString();
  path = buf.readString();
}

void FileListResponse::serialize(ByteBuffer& buf) const {
  buf.write<uint32_t>(entries.size());
  for (auto& e : entries) buf.writeString(e);
}
void FileListResponse::deserialize(ByteBuffer& buf) {
  uint32_t n = buf.read<uint32_t>();
  entries.clear();
  for (uint32_t i=0;i<n;++i) entries.push_back(buf.readString());
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
  if (!data.empty()) buf.append(reinterpret_cast<const char*>(data.data()), data.size());
}
void UploadChunk::deserialize(ByteBuffer& buf) {
  uint32_t n = buf.read<uint32_t>();
  data.resize(n);
  if (n) {
    // đọc raw bytes
    // ByteBuffer không có API readRaw, nên dùng read<uint8_t>() lặp
    for (uint32_t i=0;i<n;++i) {
      data[i] = buf.read<uint8_t>();
    }
  }
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
  if (!data.empty()) buf.append(reinterpret_cast<const char*>(data.data()), data.size());
}
void DownloadChunk::deserialize(ByteBuffer& buf) {
  uint32_t n = buf.read<uint32_t>();
  data.resize(n);
  for (uint32_t i=0;i<n;++i) {
    data[i] = buf.read<uint8_t>();
  }
}
