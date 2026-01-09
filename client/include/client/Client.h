#pragma once
#include <string>
#include "protocol/packet_header.h"
#include "protocol/bytebuffer.h"

class Client {
public:
    Client(const std::string& host, int port);
    ~Client();

    bool connectToServer();
    bool sendPacket(uint16_t type, ByteBuffer& payload);
    bool recvPacket(PacketHeader& header, ByteBuffer& payload);

    bool isConnected() const { return sockfd >= 0; }

private:
    std::string host;
    int port;
    int sockfd;
};
