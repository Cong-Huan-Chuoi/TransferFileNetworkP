#include "client/Client.h"

#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <cstdio>
#include <iostream>

Client::Client(const std::string& h, int p)
: host(h), port(p), sockfd(-1) {
    // DEBUG: kích thước PacketHeader (đảm bảo packing giống server)
    std::cout << "[Client] sizeof(PacketHeader)=" << sizeof(PacketHeader) << "\n";
}

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
    // Serialize header to network order
    PacketHeader h{};
    h.type     = htons(type);
    h.length   = htonl(static_cast<uint32_t>(payload.size()));
    h.seq      = htonl(0);
    h.checksum = htonl(0);

    // DEBUG: raw header bytes trước khi gửi
    {
        unsigned char* p = reinterpret_cast<unsigned char*>(&h);
        std::cout << "[Client] raw header:";
        for (size_t i = 0; i < sizeof(h); ++i) printf(" %02x", p[i]);
        std::cout << "\n";
    }

    if (send(sockfd, &h, sizeof(h), 0) <= 0) {
        perror("[Client] send header failed");
        return false;
    }

    // DEBUG: logical values (host order)
    std::cout << "[Client] sendPacket: type=" << type << " length=" << payload.size() << "\n";

    if (payload.size() > 0) {
        if (send(sockfd, payload.data(), payload.size(), 0) <= 0) {
            perror("[Client] send payload failed");
            return false;
        }
    }
    return true;
}

bool Client::recvPacket(PacketHeader& h, ByteBuffer& payload) {
    PacketHeader hdr_raw;
    ssize_t n = recv(sockfd, &hdr_raw, sizeof(hdr_raw), MSG_WAITALL);
    if (n <= 0) {
        perror("[Client] recv header error");
        return false;
    }

    // DEBUG: raw header bytes khi nhận
    {
        unsigned char* p = reinterpret_cast<unsigned char*>(&hdr_raw);
        std::cout << "[Client] raw header:";
        for (size_t i = 0; i < sizeof(hdr_raw); ++i) printf(" %02x", p[i]);
        std::cout << "\n";
    }

    // Convert về host order
    h.type     = ntohs(hdr_raw.type);
    h.length   = ntohl(hdr_raw.length);
    h.seq      = ntohl(hdr_raw.seq);
    h.checksum = ntohl(hdr_raw.checksum);

    // DEBUG: giá trị đã parse (host order)
    std::cout << "[Client] recvPacket: got header type=" << h.type << " length=" << h.length << "\n";

    payload.clear();
    payload = ByteBuffer(h.length);

    if (h.length > 0) {
        char buf[4096];
        size_t received = 0;
        while (received < h.length) {
            ssize_t m = recv(sockfd, buf, std::min(sizeof(buf), h.length - received), 0);
            if (m <= 0) {
                perror("[Client] recv payload error");
                return false;
            }
            payload.append(buf, m);
            received += m;
        }
    }
    return true;
}
