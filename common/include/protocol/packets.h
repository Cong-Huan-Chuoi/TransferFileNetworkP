#pragma once
#include "bytebuffer.h"
#include <string>
#include <vector>
#include <cstdint>

// ===== AUTH =====
struct RegisterRequest {
    std::string username;
    std::string password;
    void serialize(ByteBuffer& buf) const;
    void deserialize(ByteBuffer& buf);
};

struct LoginRequest {
    std::string username;
    std::string password;
    void serialize(ByteBuffer& buf) const;
    void deserialize(ByteBuffer& buf);
};

struct AuthLoginResponse { 
    bool success;
    std::string message;
    void serialize(ByteBuffer& buf) const;
    void deserialize(ByteBuffer& buf);
};

// ===== GROUP =====
struct CreateGroupRequest {
    std::string groupName;
    void serialize(ByteBuffer& buf) const;
    void deserialize(ByteBuffer& buf);
};

struct JoinGroupRequest {
    std::string groupName;
    void serialize(ByteBuffer& buf) const;
    void deserialize(ByteBuffer& buf);
};

struct ApproveJoinRequest {
    std::string groupName;
    std::string username;
    void serialize(ByteBuffer& buf) const;
    void deserialize(ByteBuffer& buf);
};

struct InviteUserRequest {
    std::string groupName;
    std::string username;
    void serialize(ByteBuffer& buf) const;
    void deserialize(ByteBuffer& buf);
};

struct AcceptInviteRequest {
    std::string groupName;
    void serialize(ByteBuffer& buf) const;
    void deserialize(ByteBuffer& buf);
};

struct LeaveGroupRequest {
    std::string groupName;
    void serialize(ByteBuffer& buf) const;
    void deserialize(ByteBuffer& buf);
};

struct KickMemberRequest {
    std::string groupName;
    std::string username;
    void serialize(ByteBuffer& buf) const;
    void deserialize(ByteBuffer& buf);
};

struct ListMembersRequest {
    std::string groupName;
    void serialize(ByteBuffer& buf) const;
    void deserialize(ByteBuffer& buf);
};

struct ListMembersResponse {
    std::vector<std::string> members;
    void serialize(ByteBuffer& buf) const;
    void deserialize(ByteBuffer& buf);
};

struct ListGroupsRequest {
    void serialize(ByteBuffer&) const {}
    void deserialize(ByteBuffer&) {}
};

struct ListGroupsResponse {
    std::vector<std::string> ownedGroups;
    std::vector<std::string> joinedGroups;
    void serialize(ByteBuffer& buf) const;
    void deserialize(ByteBuffer& buf);
};

struct PendingEntry {
    std::string groupName;
    std::string username;
    std::string inviter;
    void serialize(ByteBuffer& buf) const;
    void deserialize(ByteBuffer& buf);
};

struct PendingListResponse {
    std::vector<PendingEntry> joinRequests;
    std::vector<PendingEntry> invites;
    void serialize(ByteBuffer& buf) const;
    void deserialize(ByteBuffer& buf);
};

struct RejectJoinRequest {
    std::string groupName;
    std::string username;
    void serialize(ByteBuffer& buf) const;
    void deserialize(ByteBuffer& buf);
};

struct RejectInviteRequest {
    std::string groupName;
    std::string username;
    void serialize(ByteBuffer& buf) const;
    void deserialize(ByteBuffer& buf);
};

// ===== FILE CONTROL =====

// metadata upload (dùng cho kiểm tra + chuẩn bị)
struct UploadFileRequest {
    std::string groupName;
    std::string remotePath;
    uint64_t fileSize;

    void serialize(ByteBuffer& buf) const;
    void deserialize(ByteBuffer& buf);
};

struct DownloadFileRequest {
    std::string groupName;
    std::string remotePath;

    void serialize(ByteBuffer& buf) const;
    void deserialize(ByteBuffer& buf);
};

// list folder
struct FileListRequest {
    std::string groupName;
    std::string path; // "." hoặc subdir
    void serialize(ByteBuffer& buf) const;
    void deserialize(ByteBuffer& buf);
};

struct FileListEntry {
    std::string name;
    bool isDir;
    uint64_t size;
};

struct FileListResponse {
    std::vector<FileListEntry> entries;
    void serialize(ByteBuffer& buf) const;
    void deserialize(ByteBuffer& buf);
};

// mkdir / delete / rename / copy / move
struct MkdirRequest {
    std::string groupName;
    std::string path;
    void serialize(ByteBuffer& buf) const;
    void deserialize(ByteBuffer& buf);
};

struct DeleteRequest {
    std::string groupName;
    std::string path;
    void serialize(ByteBuffer& buf) const;
    void deserialize(ByteBuffer& buf);
};

struct RenameRequest {
    std::string groupName;
    std::string oldPath;
    std::string newPath;
    void serialize(ByteBuffer& buf) const;
    void deserialize(ByteBuffer& buf);
};

struct CopyRequest {
    std::string groupName;
    std::string srcPath;
    std::string dstPath;
    void serialize(ByteBuffer& buf) const;
    void deserialize(ByteBuffer& buf);
};

struct MoveRequest {
    std::string groupName;
    std::string srcPath;
    std::string dstPath;
    void serialize(ByteBuffer& buf) const;
    void deserialize(ByteBuffer& buf);
};

struct ActionResult {
    bool success;
    std::string message;
    void serialize(ByteBuffer& buf) const;
    void deserialize(ByteBuffer& buf);
};

// ===== FILE TRANSFER =====
// bắt đầu upload
struct UploadBegin {
    std::string groupName;
    std::string remotePath;
    uint64_t totalSize;

    void serialize(ByteBuffer& buf) const;
    void deserialize(ByteBuffer& buf);
};

// gói dữ liệu upload
struct UploadChunk {
    std::vector<uint8_t> data;

    void serialize(ByteBuffer& buf) const;
    void deserialize(ByteBuffer& buf);
};

// kết thúc upload
struct UploadEnd {
    void serialize(ByteBuffer&) const {}
    void deserialize(ByteBuffer&) {}
};

// bắt đầu download
struct DownloadBegin {
    std::string groupName;
    std::string remotePath;

    void serialize(ByteBuffer& buf) const;
    void deserialize(ByteBuffer& buf);
};

// gói dữ liệu download
struct DownloadChunk {
    std::vector<uint8_t> data;

    void serialize(ByteBuffer& buf) const;
    void deserialize(ByteBuffer& buf);
};

// kết thúc download
struct DownloadEnd {
    void serialize(ByteBuffer&) const {}
    void deserialize(ByteBuffer&) {}
};
