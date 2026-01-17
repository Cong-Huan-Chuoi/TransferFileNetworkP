#pragma once
#include <cstdint>

#pragma pack(push, 1)
struct PacketHeader {
    uint16_t type;       // PacketType
    uint32_t length;     // payload length (control data)
    uint32_t seq;        // sequence number (file chunk)
    uint32_t checksum;   // checksum of payload
};
#pragma pack(pop)
