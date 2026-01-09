#pragma once
#include <unordered_map>
#include <sys/epoll.h>
#include "SessionManager.h"
#include "protocol/packet_types.h"

class Server {
public:
    Server(int port);
    void run();

private:
    int listen_fd;
    int epoll_fd;
    SessionManager sessionManager;

    void setupSocket(int port);
    void setupEpoll();

    void acceptClient();
    void handleClientEvent(int fd);

    void recvHeader(ClientSession& session);
    void recvPayload(ClientSession& session);
    void dispatchPacket(ClientSession& session);
};
