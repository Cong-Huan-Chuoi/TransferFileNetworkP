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
