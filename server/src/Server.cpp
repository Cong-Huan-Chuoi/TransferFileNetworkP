#include "server/Server.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <cstring>

#include "protocol/packets.h"

Server::Server(int port_)
    : port(port_),
      authManager("data/users.db"),
      groupManager("data/groups.db"),
      fsManager("data/groups"),
      fileReceiver(fsManager),
      permissionChecker(sessionManager, groupManager),
      logger("data/server.log") {
    setup_socket();
}

Server::~Server() {
    close(listen_fd);
    close(epoll_fd);
}

void Server::setup_socket() {
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(listen_fd, (sockaddr*)&addr, sizeof(addr));
    listen(listen_fd, 16);

    epoll_fd = epoll_create1(0);

    epoll_event ev{};
    ev.events = EPOLLIN;
    ev.data.fd = listen_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &ev);
}

void Server::run() {
    epoll_event events[32];
    logger.info("Server started");

    while (true) {
        int n = epoll_wait(epoll_fd, events, 32, -1);
        for (int i = 0; i < n; ++i) {
            int fd = events[i].data.fd;
            if (fd == listen_fd)
                handle_accept();
            else
                handle_client(fd);
        }
    }
}

void Server::handle_accept() {
    int fd = accept(listen_fd, nullptr, nullptr);

    epoll_event ev{};
    ev.events = EPOLLIN;
    ev.data.fd = fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev);

    sessionManager.add_session(fd);
    logger.info("Client connected fd=" + std::to_string(fd));
}

void Server::handle_client(int fd) {
    uint8_t buf[4096];
    int n = recv(fd, buf, sizeof(buf), 0);

    if (n <= 0) {
        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
        sessionManager.remove_session(fd);
        logger.info("Client disconnected fd=" + std::to_string(fd));
        return;
    }

    ClientSession* sess = sessionManager.get_session(fd);
    sess->buffer.append(buf, n);

    PacketHeader h;
    std::vector<uint8_t> payload;

    while (try_parse_packet(sess->buffer, h, payload)) {
        handle_packet(fd, h, payload);
    }
}

void Server::send_ok(int fd, const std::string& msg) {
    std::vector<uint8_t> data(msg.begin(), msg.end());
    auto pkt = make_packet(PacketType::OK, 0, 0, data);
    send(fd, pkt.data(), pkt.size(), 0);
}

void Server::send_error(int fd, const std::string& msg) {
    std::vector<uint8_t> data(msg.begin(), msg.end());
    auto pkt = make_packet(PacketType::ERROR, FLAG_ERR, 0, data);
    send(fd, pkt.data(), pkt.size(), 0);
}
