#pragma once
#include "bytebuffer.h"
#include <string>
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
  bool success; std::string message; 
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
    void serialize(ByteBuffer& buf) const {}
    void deserialize(ByteBuffer& buf) {}
};

struct ListGroupsResponse {
    std::vector<std::string> ownedGroups;   // nhóm mà user là owner
    std::vector<std::string> joinedGroups;  // nhóm mà user là member

    void serialize(ByteBuffer& buf) const;
    void deserialize(ByteBuffer& buf);
};




// ===== FILE CONTROL =====
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


// ===== FILE CONTROL =====
struct FileListRequest {
  std::string groupName;
  std::string path; // "." hoặc subdir
  void serialize(ByteBuffer& buf) const;
  void deserialize(ByteBuffer& buf);
};

struct FileListResponse {
  std::vector<std::string> entries;
  void serialize(ByteBuffer& buf) const;
  void deserialize(ByteBuffer& buf);
};

struct MkdirRequest {
  std::string groupName;
  std::string path; // subdir to create
  void serialize(ByteBuffer& buf) const;
  void deserialize(ByteBuffer& buf);
};

struct DeleteRequest {
  std::string groupName;
  std::string path; // file/dir to delete
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

// generic small result
struct ActionResult {
  bool success;
  std::string message;
  void serialize(ByteBuffer& buf) const;
  void deserialize(ByteBuffer& buf);
};

// ===== FILE TRANSFER =====
struct UploadBegin {
  std::string groupName;
  std::string remotePath;     // target path under group dir
  uint64_t totalSize;         // total file size
  void serialize(ByteBuffer& buf) const;
  void deserialize(ByteBuffer& buf);
};

struct UploadChunk {
  std::vector<uint8_t> data;  // chunk bytes
  void serialize(ByteBuffer& buf) const;
  void deserialize(ByteBuffer& buf);
};

struct UploadEnd {
  void serialize(ByteBuffer& buf) const {}
  void deserialize(ByteBuffer& buf) {}
};

struct DownloadBegin {
  std::string groupName;
  std::string remotePath;     // source path under group dir
  void serialize(ByteBuffer& buf) const;
  void deserialize(ByteBuffer& buf);
};

struct DownloadChunk {
  std::vector<uint8_t> data;  // chunk bytes
  void serialize(ByteBuffer& buf) const;
  void deserialize(ByteBuffer& buf);
};

struct DownloadEnd {
  void serialize(ByteBuffer& buf) const {}
  void deserialize(ByteBuffer& buf) {}
};
