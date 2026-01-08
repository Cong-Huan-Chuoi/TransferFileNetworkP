#include "client/Client.h"
#include "client/AuthClient.h"
#include "client/GroupClient.h"
#include "client/FileClient.h"
#include "protocol/packets.h"
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

Client::Client(const std::string& host, int port) {
    sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &addr.sin_addr);

    connect(sock, (sockaddr*)&addr, sizeof(addr));

    auth  = new AuthClient(*this);
    group = new GroupClient(*this);
    file  = new FileClient(*this);
}

Client::~Client() {
    close(sock);
    delete auth;
    delete group;
    delete file;
}

void Client::send_packet(PacketType type, const std::string& payload) {
    std::vector<uint8_t> data(payload.begin(), payload.end());
    auto pkt = make_packet(type, 0, 0, data);
    send(sock, pkt.data(), pkt.size(), 0);
}

bool Client::recv_packet(PacketHeader& h, std::vector<uint8_t>& payload) {
    uint8_t buf[4096];
    int n = recv(sock, buf, sizeof(buf), 0);
    if (n <= 0) return false;

    buffer.append(buf, n);
    return try_parse_packet(buffer, h, payload);
}

void Client::run() {
    std::string cmd;

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, cmd);

        if (auth->handle(cmd)) continue;
        if (group->handle(cmd)) continue;
        if (file->handle(cmd)) continue;

        std::cout << "Unknown command\n";
    }
}
