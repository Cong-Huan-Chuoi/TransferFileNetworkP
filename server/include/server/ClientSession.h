#pragma once
#include <string>
#include "protocol/bytebuffer.h"
#include "protocol/packet_header.h"

enum class RecvState {
    READ_HEADER,
    READ_PAYLOAD,
    READ_FILE
};

struct ClientSession {
    int fd;
    bool logged_in = false;
    std::string username;
    std::string current_group;

    // recv state
    RecvState state = RecvState::READ_HEADER;
    PacketHeader currentHeader{};
    ByteBuffer payloadBuffer;

    // file streaming
    bool receiving_file = false;
    uint64_t remaining_file_bytes = 0;
};
