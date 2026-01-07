#include "protocol/packets.h"
#include "protocol/checksum.h"
#include <cstring>

std::vector<uint8_t> make_packet(
    PacketType type,
    uint16_t flags,
    uint32_t sequence,
    const std::vector<uint8_t>& payload
) {
    PacketHeader header{};
    header.type = type;
    header.flags = flags;
    header.sequence = sequence;
    header.payload_size = payload.size();
    header.checksum = payload.empty()
        ? 0
        : simple_checksum(payload.data(), payload.size());

    std::vector<uint8_t> packet(sizeof(PacketHeader) + payload.size());
    std::memcpy(packet.data(), &header, sizeof(PacketHeader));

    if (!payload.empty()) {
        std::memcpy(
            packet.data() + sizeof(PacketHeader),
            payload.data(),
            payload.size()
        );
    }

    return packet;
}

bool try_parse_packet(
    ByteBuffer& buffer,
    PacketHeader& header,
    std::vector<uint8_t>& payload
) {
    if (buffer.size() < sizeof(PacketHeader))
        return false;

    // Read header
    if (!buffer.read(&header, sizeof(PacketHeader)))
        return false;

    // Not enough payload yet
    if (buffer.size() < header.payload_size)
        return false;

    payload.resize(header.payload_size);
    if (header.payload_size > 0) {
        buffer.read(payload.data(), header.payload_size);
    }

    // Verify checksum
    uint32_t cs = payload.empty()
        ? 0
        : simple_checksum(payload.data(), payload.size());

    return cs == header.checksum;
}

