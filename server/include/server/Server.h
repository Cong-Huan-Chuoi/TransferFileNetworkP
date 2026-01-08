#pragma once

#include <vector>
#include <string>

#include "protocol/packet_header.h"

#include "server/AuthManager.h"
#include "server/SessionManager.h"
#include "server/GroupManager.h"
#include "server/FileSystemManager.h"
#include "server/FileReceiver.h"
#include "server/PermissionChecker.h"
#include "server/Logger.h"

class Server {
public:
    explicit Server(int port);
    ~Server();

    void run();

private:
    // socket / epoll
    void setup_socket();
    void handle_accept();
    void handle_client(int fd);

    // protocol
    void handle_packet(int fd,
                       const PacketHeader& header,
                       const std::vector<uint8_t>& payload);

    void send_ok(int fd, const std::string& msg = "");
    void send_error(int fd, const std::string& msg);

private:
    int port;
    int listen_fd;
    int epoll_fd;

    SessionManager sessionManager;
    AuthManager authManager;
    GroupManager groupManager;
    FileSystemManager fsManager;
    FileReceiver fileReceiver;
    PermissionChecker permissionChecker;
    Logger logger;
};
