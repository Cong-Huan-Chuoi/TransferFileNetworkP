#include "server/Server.h"

#include "server/SessionManager.h"
#include "server/AuthManager.h"
#include "server/GroupManager.h"
#include "server/PermissionChecker.h"
#include "server/FileSystemManager.h"
#include "server/Logger.h"
#include "server/DataPaths.h"

#include "protocol/packet_types.h"
#include "protocol/packets.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <memory>

// ================== CORE MANAGERS (POINTER-BASED) ==================

static std::unique_ptr<AuthManager> authManager;
static std::unique_ptr<GroupManager> groupManager;
static std::unique_ptr<FileSystemManager> fsManager;
static std::unique_ptr<PermissionChecker> permissionChecker;
static Logger logger("data/server.log");

static bool coreInitialized = false;

// ================== CORE INIT ==================

static void initCoreOnce() {
    if (coreInitialized) return;

    authManager = std::make_unique<AuthManager>(DataPaths::usersDb());
    groupManager = std::make_unique<GroupManager>(DataPaths::groupsDb());
    fsManager = std::make_unique<FileSystemManager>(DataPaths::groupsDir());
    permissionChecker =
        std::make_unique<PermissionChecker>(*groupManager);

    coreInitialized = true;
}

// ================== UTILS ==================

static void setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

// ================== SERVER ==================

Server::Server(int port) {
    initCoreOnce();              // 🔒 SAFE CORE INIT

    setupSocket(port);
    setupEpoll();
}

void Server::setupSocket(int port) {
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    bind(listen_fd, (sockaddr*)&addr, sizeof(addr));
    listen(listen_fd, SOMAXCONN);

    setNonBlocking(listen_fd);
}

void Server::setupEpoll() {
    epoll_fd = epoll_create1(0);

    epoll_event ev{};
    ev.events = EPOLLIN;
    ev.data.fd = listen_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &ev);
}

void Server::run() {
    epoll_event events[64];

    while (true) {
        int n = epoll_wait(epoll_fd, events, 64, -1);
        for (int i = 0; i < n; ++i) {
            int fd = events[i].data.fd;

            if (fd == listen_fd) {
                acceptClient();
            } else {
                handleClientEvent(fd);
            }
        }
    }
}

// ================== ACCEPT / EVENT ==================

void Server::acceptClient() {
    int client_fd = accept(listen_fd, nullptr, nullptr);
    if (client_fd < 0) return;

    setNonBlocking(client_fd);

    epoll_event ev{};
    ev.events = EPOLLIN;
    ev.data.fd = client_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev);

    sessionManager.addSession(client_fd);
}

void Server::handleClientEvent(int fd) {
    ClientSession* session = sessionManager.getSession(fd);
    if (!session) return;

    try {
        if (session->state == RecvState::READ_HEADER) {
            recvHeader(*session);
        } else if (session->state == RecvState::READ_PAYLOAD) {
            recvPayload(*session);
        }
    } catch (...) {
        sessionManager.removeSession(fd);
    }
}

// ================== RECV ==================

void Server::recvHeader(ClientSession& session) {
    ssize_t n = recv(session.fd,
                     &session.currentHeader,
                     sizeof(PacketHeader),
                     0);
    if (n <= 0)
        throw std::runtime_error("disconnect");

    session.payloadBuffer.clear();
    session.payloadBuffer = ByteBuffer(session.currentHeader.length);
    session.state = RecvState::READ_PAYLOAD;
}

void Server::recvPayload(ClientSession& session) {
    size_t need =
        session.currentHeader.length - session.payloadBuffer.size();

    char buf[4096];
    ssize_t n = recv(session.fd,
                     buf,
                     std::min(sizeof(buf), need),
                     0);

    if (n <= 0)
        throw std::runtime_error("disconnect");

    session.payloadBuffer.append(buf, n);

    if (session.payloadBuffer.size() ==
        session.currentHeader.length) {
        dispatchPacket(session);
        session.state = RecvState::READ_HEADER;
    }
}

// ================== DISPATCH ==================

void Server::dispatchPacket(ClientSession& session) {
    PacketType type =
        static_cast<PacketType>(session.currentHeader.type);

    ByteBuffer& buf = session.payloadBuffer;

    // ===== AUTH =====
    if (type == PacketType::AUTH_REGISTER_REQ) {
        RegisterRequest req;
        req.deserialize(buf);
        authManager->registerUser(req.username, req.password);
        return;
    }

    if (type == PacketType::AUTH_LOGIN_REQ) {
        LoginRequest req;
        req.deserialize(buf);

        AuthResult res =
            authManager->verifyLogin(req.username, req.password);

        if (res == AuthResult::SUCCESS) {
            session.logged_in = true;
            session.username = req.username;
        }
        return;
    }

    if (!session.logged_in) return;

    // ===== GROUP =====
    if (type == PacketType::GROUP_CREATE_REQ) {
        CreateGroupRequest req;
        req.deserialize(buf);
        groupManager->createGroup(req.groupName, session.username);
        logger.log("CREATE_GROUP " + req.groupName);
        return;
    }

    if (type == PacketType::GROUP_JOIN_REQ) {
        JoinGroupRequest req;
        req.deserialize(buf);
        groupManager->requestJoin(req.groupName, session.username);
        logger.log("JOIN_GROUP " + req.groupName);
        return;
    }

    // ===== FILE SYSTEM (DEMO) =====
    if (type == PacketType::FILE_LIST_REQ) {
        if (permissionChecker->canPerform(
                session.username,
                session.current_group,
                FileAction::LIST)) {

            fsManager->list(session.current_group, ".");
            logger.log("LIST_FILES");
        }
        return;
    }

    std::cout << "Unknown packet type\n";
}
