#pragma once
#include <cstdint>

enum class PacketType : uint16_t {
    // ===== AUTH =====
    AUTH_REGISTER_REQ = 1,
    AUTH_REGISTER_RES,
    AUTH_LOGIN_REQ,
    AUTH_LOGIN_RES,

    // ===== GROUP =====
    GROUP_CREATE_REQ,
    GROUP_CREATE_RES,
    GROUP_JOIN_REQ,
    GROUP_JOIN_RES,
    GROUP_APPROVE_REQ,
    GROUP_APPROVE_RES,

    // ===== FILE SYSTEM =====
    FILE_LIST_REQ,
    FILE_LIST_RES,

    FILE_MKDIR_REQ,
    FILE_MKDIR_RES,

    FILE_DELETE_REQ,
    FILE_DELETE_RES,

    // ===== FILE TRANSFER =====
    FILE_UPLOAD_REQ,
    FILE_UPLOAD_RES,

    FILE_DOWNLOAD_REQ,
    FILE_DOWNLOAD_RES
};
