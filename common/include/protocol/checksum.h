#pragma once
#include <cstdint>
#include <cstddef>

class Checksum {
public:
    static uint32_t simple(const uint8_t* data, size_t len) {
        uint32_t sum = 0;
        for (size_t i = 0; i < len; ++i)
            sum += data[i];
        return sum;
    }
};
