#pragma once
#include <cstdint>

/*
 * Định nghĩa các loại packet trong giao thức
 * FE & BE dùng chung
 */
enum class PacketType : uint16_t {
    // Upload
    UPLOAD_REQUEST = 1,
    UPLOAD_CHUNK   = 2,
    UPLOAD_FINISH  = 3,

    // Download 
    DOWNLOAD_REQUEST = 10,
    DOWNLOAD_CHUNK   = 11,
    DOWNLOAD_FINISH  = 12,

    // Control
    OK    = 100,
    ERROR = 101
};
