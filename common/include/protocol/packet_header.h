#pragma once
#include<cstdint>
#include "packet_types.h"

// PacketHeader 16 bytes:  type (2B)│ flags (2B)│ seq (4B)│ payload (4B)│ checksum (4B)


#pragma pack(push, 1)

struct PacketHeader{
    PacketType type; 
    uint16_t flags;
    uint32_t sequence;
    uint32_t payload_size;
    uint32_t checksum;
};

#pragma pack(pop)

/* Flags */
constexpr uint16_t FLAG_NONE       = 0x0000;
constexpr uint16_t FLAG_LAST_CHUNK = 0x0001;
constexpr uint16_t FLAG_ERROR      = 0x0002;