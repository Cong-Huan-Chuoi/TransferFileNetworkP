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
