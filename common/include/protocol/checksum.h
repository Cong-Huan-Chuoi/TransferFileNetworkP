#pragma once
#include <cstdint>
#include <cstddef>

/*
 * Simple checksum cho payload
 * Nhẹ, nhanh, đủ dùng cho bài Network Programming
 */
inline uint32_t simple_checksum(const uint8_t* data, size_t len) {
    uint32_t sum = 0;
    for (size_t i = 0; i < len; ++i) {
        sum += data[i];
    }
    return sum;
}