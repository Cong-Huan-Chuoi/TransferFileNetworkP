#pragma once
#include <cstdint>

/*
 * Định nghĩa các loại packet trong giao thức
 * FE & BE dùng chung
 */
enum class PacketType : uint16_t {
    // Auth
    REGISTER = 1,
    LOGIN    = 2,
    LOGOUT   = 3,

    // Group
    CREATE_GROUP = 10,
    LIST_GROUPS  = 11,
    JOIN_GROUP_REQUEST = 12,
    APPROVE_JOIN = 13,
    INVITE_MEMBER = 14,
    LEAVE_GROUP = 15,
    KICK_MEMBER = 16,
    LIST_MEMBERS = 17,

    // File / Dir
    LIST_DIR = 30,
    MKDIR    = 31,
    RENAME   = 32,
    DELETE_  = 33,
    MOVE     = 34,
    COPY     = 35,

    // Transfer
    UPLOAD_REQUEST   = 50,
    UPLOAD_CHUNK     = 51,
    UPLOAD_FINISH    = 52,
    DOWNLOAD_REQUEST = 53,
    DOWNLOAD_CHUNK   = 54,
    DOWNLOAD_FINISH  = 55,

    // Control
    OK    = 200,
    ERROR = 201
};
