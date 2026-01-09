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
