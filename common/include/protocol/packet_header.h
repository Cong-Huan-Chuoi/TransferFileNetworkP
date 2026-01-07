#pragma once
#include <cstdint>
#include "packet_types.h"

/*
 * PacketHeader (20 bytes)
 *
 * | type(2) | flags(2) | seq(4) | payload_len(4) | checksum(8) |
 */
#pragma pack(push, 1)
struct PacketHeader {
    PacketType type;        // 2 bytes
    uint16_t   flags;       // 2 bytes
    uint32_t   seq;         // chunk sequence
    uint32_t   payload_len;// payload size
    uint64_t   checksum;   // checksum payload
};
#pragma pack(pop)

constexpr uint16_t FLAG_NONE = 0;
constexpr uint16_t FLAG_LAST = 1 << 0;
constexpr uint16_t FLAG_ERR  = 1 << 1;
