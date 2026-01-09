#include "server/Server.h"

#include "server/SessionManager.h"
#include "server/AuthManager.h"
#include "server/GroupManager.h"
#include "server/PermissionChecker.h"
#include "server/FileSystemManager.h"
#include "server/Logger.h"

#include "server/services/AuthService.h"
#include "server/services/GroupService.h"
#include "server/services/FileService.h"

#include "protocol/packet_types.h"
#include "protocol/packets.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <algorithm>

#include "server/DataPaths.h"

// ================== CORE SINGLETON ==================

static bool coreInitialized = false;

static AuthManager* authManager = nullptr;
static GroupManager* groupManager = nullptr;
static FileSystemManager* fsManager = nullptr;
static PermissionChecker* permissionChecker = nullptr;
static Logger* logger = nullptr;

static AuthService* authService = nullptr;
static GroupService* groupService = nullptr;
static FileService* fileService = nullptr;

static void initCoreOnce() {
    if (coreInitialized) return;

    authManager = new AuthManager(DataPaths::usersDb());
    groupManager = new GroupManager(DataPaths::groupsDb());
    fsManager = new FileSystemManager(DataPaths::groupsDir());
    permissionChecker = new PermissionChecker(*groupManager);
    logger = new Logger("data/server.log");

    authService = new AuthService(*authManager);
    groupService = new GroupService(*groupManager);
    fileService = new FileService(*fsManager, *permissionChecker);

    coreInitialized = true;
}

// ================== UTILS ==================

static void setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

// ================== SERVER ==================

Server::Server(int port) {
    initCoreOnce();
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
        authService->registerUser(req.username, req.password);
        return;
    }

    if (type == PacketType::AUTH_LOGIN_REQ) {
        LoginRequest req;
        req.deserialize(buf);
        if (authService->login(req.username, req.password)) {
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
        groupService->createGroup(session.username, req.groupName);
        logger->log("CREATE_GROUP " + req.groupName);
        return;
    }

    if (type == PacketType::GROUP_JOIN_REQ) {
        JoinGroupRequest req;
        req.deserialize(buf);
        groupService->requestJoin(session.username, req.groupName);
        logger->log("JOIN_GROUP " + req.groupName);
        return;
    }

    if (type == PacketType::GROUP_APPROVE_REQ) {
        ApproveJoinRequest req;
        req.deserialize(buf);
        groupService->approveJoin(session.username,
                                  req.username,
                                  req.groupName);
        logger->log("APPROVE_JOIN " + req.groupName);
        return;
    }

    if (type == PacketType::GROUP_INVITE_REQ) {
        InviteUserRequest req;
        req.deserialize(buf);
        groupService->inviteUser(session.username,
                                 req.username,
                                 req.groupName);
        logger->log("INVITE_USER " + req.username);
        return;
    }

    if (type == PacketType::GROUP_LEAVE_REQ) {
        LeaveGroupRequest req;
        req.deserialize(buf);
        groupService->leaveGroup(session.username, req.groupName);
        logger->log("LEAVE_GROUP " + req.groupName);
        return;
    }

    if (type == PacketType::GROUP_KICK_REQ) {
        KickMemberRequest req;
        req.deserialize(buf);
        groupService->kickUser(session.username,
                               req.username,
                               req.groupName);
        logger->log("KICK_MEMBER " + req.username);
        return;
    }

    if (type == PacketType::GROUP_LIST_MEMBERS_REQ) {
        ListMembersRequest req;
        req.deserialize(buf);
        auto members =
            groupService->listMembers(session.username,
                                      req.groupName);

        ListMembersResponse res{members};
        ByteBuffer outBuf;
        res.serialize(outBuf);

        PacketHeader hdr{};
        hdr.type = static_cast<uint16_t>(
            PacketType::GROUP_LIST_MEMBERS_RES);
        hdr.length = outBuf.size();

        send(session.fd, &hdr, sizeof(hdr), 0);
        send(session.fd, outBuf.data(), outBuf.size(), 0);
        return;
    }

    if (type == PacketType::GROUP_LIST_REQ) {
        ListGroupsResponse res;
        res.ownedGroups =
            groupService->listOwnedGroups(session.username);
        res.joinedGroups =
            groupService->listJoinedGroups(session.username);

        ByteBuffer outBuf;
        res.serialize(outBuf);

        PacketHeader hdr{};
        hdr.type =
            static_cast<uint16_t>(PacketType::GROUP_LIST_RES);
        hdr.length = outBuf.size();

        send(session.fd, &hdr, sizeof(hdr), 0);
        send(session.fd, outBuf.data(), outBuf.size(), 0);
        return;
    }

    // ===== FILE =====
    if (type == PacketType::FILE_LIST_REQ) {
        fileService->list(session.username,
                          session.current_group,
                          ".");
        return;
    }

    if (type == PacketType::FILE_MKDIR_REQ) {
        logger->log("MKDIR");
        return;
    }

    if (type == PacketType::FILE_DELETE_REQ) {
        logger->log("DELETE");
        return;
    }
}
