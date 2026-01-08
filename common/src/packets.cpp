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
    // 1. Peek header
    if (!buffer.can_read(sizeof(PacketHeader)))
        return false;

    PacketHeader temp;
    if (!buffer.peek(&temp, sizeof(PacketHeader)))
        return false;

    // 2. Validate payload_len
    constexpr uint32_t MAX_PAYLOAD = 16 * 1024 * 1024; // 16MB
    if (temp.payload_len > MAX_PAYLOAD)
        return false; // protocol error

    // 3. Check full packet
    if (!buffer.can_read(sizeof(PacketHeader) + temp.payload_len))
        return false;

    // 4. Now we can consume
    buffer.read(&header, sizeof(PacketHeader));

    payload.resize(header.payload_len);
    if (header.payload_len > 0)
        buffer.read(payload.data(), header.payload_len);

    // 5. Verify checksum
    uint64_t cs = payload.empty()
        ? 0
        : simple_checksum(payload.data(), payload.size());

    return cs == header.checksum;
}


