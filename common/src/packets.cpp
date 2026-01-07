#include "protocol/packets.h"
#include "protocol/checksum.h"
#include <cstring>

std::vector<uint8_t> make_packet(
    PacketType type,
    uint16_t flags,
    uint32_t seq,
    const std::vector<uint8_t>& payload
) {
    PacketHeader h{};
    h.type = type;
    h.flags = flags;
    h.seq = seq;
    h.payload_len = payload.size();
    h.checksum = payload.empty()
        ? 0
        : simple_checksum(payload.data(), payload.size());

    std::vector<uint8_t> pkt(sizeof(PacketHeader) + payload.size());
    std::memcpy(pkt.data(), &h, sizeof(PacketHeader));

    if (!payload.empty()) {
        std::memcpy(pkt.data() + sizeof(PacketHeader),
                    payload.data(), payload.size());
    }
    return pkt;
}

bool try_parse_packet(
    ByteBuffer& buffer,
    PacketHeader& header,
    std::vector<uint8_t>& payload
) {
    if (!buffer.can_read(sizeof(PacketHeader)))
        return false;

    if (!buffer.read(&header, sizeof(PacketHeader)))
        return false;

    if (!buffer.can_read(header.payload_len))
        return false;

    payload.resize(header.payload_len);
    if (header.payload_len > 0)
        buffer.read(payload.data(), header.payload_len);

    uint64_t cs = payload.empty()
        ? 0
        : simple_checksum(payload.data(), payload.size());

    return cs == header.checksum;
}


