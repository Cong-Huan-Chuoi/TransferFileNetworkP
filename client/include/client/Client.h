#pragma once

#include <string>
#include "protocol/bytebuffer.h"
#include "client/Session.h"
#include "protocol/packet_header.h"

class AuthClient;
class GroupClient;
class FileClient;

class Client {
public:
    Client(const std::string& host, int port);
    ~Client();

    void run();

    // dùng chung cho sub-client
    void send_packet(PacketType type, const std::string& payload);
    bool recv_packet(PacketHeader& h, std::vector<uint8_t>& payload);

    Session& session() { return sess; }

private:
    int sock;
    ByteBuffer buffer;
    Session sess;

    AuthClient* auth;
    GroupClient* group;
    FileClient* file;
};
