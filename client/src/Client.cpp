#include "client/Client.h"
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

Client::Client(const std::string& h, int p)
    : host(h), port(p), sockfd(-1) {}

Client::~Client() {
    if (sockfd >= 0) close(sockfd);
}

bool Client::connectToServer() {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) return false;

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &addr.sin_addr);

    return connect(sockfd, (sockaddr*)&addr, sizeof(addr)) == 0;
}

bool Client::sendPacket(uint16_t type, ByteBuffer& payload) {
    PacketHeader h{};
    h.type = type;
    h.length = payload.size();
    h.seq = 0;
    h.checksum = 0;

    if (send(sockfd, &h, sizeof(h), 0) <= 0) return false;
    if (payload.size() > 0)
        if (send(sockfd, payload.data(), payload.size(), 0) <= 0)
            return false;
    return true;
}

bool Client::recvPacket(PacketHeader& h, ByteBuffer& payload) {
    if (recv(sockfd, &h, sizeof(h), MSG_WAITALL) <= 0)
        return false;

    payload.clear();
    payload = ByteBuffer(h.length);

    if (h.length > 0) {
        char buf[4096];
        size_t received = 0;
        while (received < h.length) {
            ssize_t n = recv(sockfd, buf,
                std::min(sizeof(buf), h.length - received), 0);
            if (n <= 0) return false;
            payload.append(buf, n);
            received += n;
        }
    }
    return true;
}
