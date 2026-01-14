#pragma once
#include <string>

#include "protocol/packet_header.h"
#include "protocol/bytebuffer.h"
#include "client/Session.h"

class Client {
public:
    Client(const std::string& host, int port);
    ~Client();

    bool connectToServer();
    bool sendPacket(uint16_t type, ByteBuffer& payload);
    bool recvPacket(PacketHeader& header, ByteBuffer& payload);

    bool isConnected() const { return sockfd >= 0; }

    // ===== SESSION ACCESS =====
    Session& getSession() { return session; }
    const Session& getSession() const { return session; }

private:
    std::string host;
    int port;
    int sockfd;

    Session session;   
};
