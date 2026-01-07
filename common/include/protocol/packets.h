
//  * Tạo packet hoàn chỉnh để gửi

//  * Thử parse 1 packet từ ByteBuffer
//  * Trả true nếu parse thành công

#pragma once
#include <vector>
#include <cstdint>

#include "packet_header.h"
#include "bytebuffer.h"

std::vector<uint8_t> make_packet(
    PacketType type,
    uint16_t flags,
    uint32_t seq,
    const std::vector<uint8_t>& payload
);

bool try_parse_packet(
    ByteBuffer& buffer,
    PacketHeader& header,
    std::vector<uint8_t>& payload
);
