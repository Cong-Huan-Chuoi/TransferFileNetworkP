#include "server/Server.h"

#include "server/SessionManager.h"
#include "server/AuthManager.h"
#include "server/GroupManager.h"
#include "server/PermissionChecker.h"
#include "server/FileSystemManager.h"
#include "server/Logger.h"

#include "protocol/packet_types.h"
#include "protocol/packets.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <unordered_map>
#include "server/DataPaths.h"
#include <unordered_map>

namespace fs = std::filesystem;

// ================== GLOBAL MANAGERS ==================

static AuthManager authManager(DataPaths::usersDb());
static GroupManager groupManager(DataPaths::groupsDb());
static PermissionChecker permissionChecker(groupManager);
static FileSystemManager fsManager(DataPaths::groupsDir());
static Logger logger((std::filesystem::path(DataPaths::dataDir()) / "server.log").string());

// ================== UTILS ==================

static void setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

// ===== FILE UPLOAD STREAM STATE (GLOBAL TO SERVER.cpp) =====
static std::unordered_map<int, std::ofstream> uploadStreams;
static std::unordered_map<int, uint64_t> uploadRemaining;

// ================== SERVER ==================

Server::Server(int port) {
    setupSocket(port);
    setupEpoll();
    // DEBUG: kích thước PacketHeader (đảm bảo packing giống client)
    std::cout << "[Server] sizeof(PacketHeader)=" << sizeof(PacketHeader) << "\n";
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
    PacketHeader hdr_raw;
    ssize_t n = recv(session.fd, &hdr_raw, sizeof(hdr_raw), 0);
    if (n <= 0)
        throw std::runtime_error("disconnect");

    // DEBUG: in raw header bytes (network order)
    {
        unsigned char* p = reinterpret_cast<unsigned char*>(&hdr_raw);
        std::cout << "[Server] raw header:";
        for (size_t i = 0; i < sizeof(hdr_raw); ++i) printf(" %02x", p[i]);
        std::cout << "\n";
    }

    // network → host
    PacketHeader hdr;
    hdr.type     = ntohs(hdr_raw.type);
    hdr.length   = ntohl(hdr_raw.length);
    hdr.seq      = ntohl(hdr_raw.seq);
    hdr.checksum = ntohl(hdr_raw.checksum);

    session.currentHeader = hdr;

    // FIX: nếu không có payload, dispatch ngay
    if (hdr.length == 0) {
        session.payloadBuffer.clear();
        std::cout << "[Server] dispatch immediately (length=0)\n"; // DEBUG
        dispatchPacket(session);
        session.state = RecvState::READ_HEADER;
        return;
    }

    // Có payload → chuẩn bị buffer
    session.payloadBuffer.clear();
    session.payloadBuffer = ByteBuffer(hdr.length);
    session.state = RecvState::READ_PAYLOAD;
}

void Server::recvPayload(ClientSession& session) {
    size_t need = session.currentHeader.length - session.payloadBuffer.size();

    char buf[4096];
    ssize_t n = recv(session.fd, buf, std::min(sizeof(buf), need), 0);
    if (n <= 0)
        throw std::runtime_error("disconnect");

    session.payloadBuffer.append(buf, n);

    if (session.payloadBuffer.size() == session.currentHeader.length) {
        dispatchPacket(session);
        session.state = RecvState::READ_HEADER;
    }
}

// ================== DISPATCH ==================

void Server::dispatchPacket(ClientSession& session) {
    PacketType type = static_cast<PacketType>(session.currentHeader.type);
    ByteBuffer& buf = session.payloadBuffer;

    // DEBUG: in thông tin packet đã parse (host order)
    std::cout << "[Server] dispatchPacket: fd=" << session.fd
              << " type=" << static_cast<int>(session.currentHeader.type)
              << " length=" << session.currentHeader.length
              << " username=" << session.username
              << " logged_in=" << (session.logged_in ? "1" : "0") << "\n";
    logger.log("DISPATCH fd=" + std::to_string(session.fd)
               + " type=" + std::to_string(session.currentHeader.type)
               + " user=" + session.username
               + " logged_in=" + (session.logged_in ? "1" : "0"));

    // ===== AUTH =====
    if (type == PacketType::AUTH_REGISTER_REQ) {
        RegisterRequest req;
        req.deserialize(buf);
        AuthResult result = authManager.registerUser(req.username, req.password);

        ActionResult resPkt;
        resPkt.success = (result == AuthResult::SUCCESS);
        if (result == AuthResult::SUCCESS) resPkt.message = "Register success";
        else if (result == AuthResult::USER_EXISTS) resPkt.message = "User already exists";
        else resPkt.message = "Register failed";

        ByteBuffer out;
        resPkt.serialize(out);

        PacketHeader hdr_net{};
        hdr_net.type = htons(static_cast<uint16_t>(PacketType::AUTH_REGISTER_RES));
        hdr_net.length = htonl(static_cast<uint32_t>(out.size()));
        hdr_net.seq = htonl(0);
        hdr_net.checksum = htonl(0);

        send(session.fd, &hdr_net, sizeof(hdr_net), 0);
        if (out.size() > 0) send(session.fd, out.data(), out.size(), 0);

        logger.log("REGISTER " + req.username + " result: " + resPkt.message);
        return;
    }


    if (type == PacketType::AUTH_LOGIN_REQ) {
        LoginRequest req;
        req.deserialize(buf);

        AuthResult result = authManager.verifyLogin(req.username, req.password);
        bool ok = (result == AuthResult::SUCCESS);

        if (ok) {
            session.logged_in = true;
            session.username = req.username;
        }

        AuthLoginResponse resPkt;
        resPkt.success = ok;
        resPkt.message = ok ? "Login success" : "Login failed";

        ByteBuffer out;
        resPkt.serialize(out);

        // Gửi header ở network order
        PacketHeader hdr_net{};
        hdr_net.type     = htons(static_cast<uint16_t>(PacketType::AUTH_LOGIN_RES));
        hdr_net.length   = htonl(static_cast<uint32_t>(out.size()));
        hdr_net.seq      = htonl(0);
        hdr_net.checksum = htonl(0);

        // DEBUG: raw header sẽ gửi
        {
            unsigned char* p = reinterpret_cast<unsigned char*>(&hdr_net);
            std::cout << "[Server] raw header to send:";
            for (size_t i = 0; i < sizeof(hdr_net); ++i) printf(" %02x", p[i]);
            std::cout << "\n";
        }

        send(session.fd, &hdr_net, sizeof(hdr_net), 0);
        if (out.size() > 0) send(session.fd, out.data(), out.size(), 0);

        logger.log("LOGIN " + req.username + " result: " + (ok ? "SUCCESS" : "FAIL"));
        return;
    }

    // ===== REQUIRE LOGIN =====
    if (!session.logged_in) {
        return;
    }

    // ===== GROUP =====
    if (type == PacketType::GROUP_CREATE_REQ) {
        CreateGroupRequest req; req.deserialize(buf);
        bool ok = groupManager.createGroup(req.groupName, session.username);
        logger.log("CREATE_GROUP " + req.groupName + " by " + session.username);

        if (ok) {
            try {
                fs::create_directories(fs::path(DataPaths::groupsDir()) / req.groupName);
            } catch (...) {
                logger.log("CREATE_GROUP_DIR_FAIL " + req.groupName);
            }
        }
        return;
    }

    if (type == PacketType::GROUP_JOIN_REQ) {
        JoinGroupRequest req; req.deserialize(buf);
        groupManager.requestJoin(req.groupName, session.username);
        logger.log("JOIN_GROUP " + req.groupName + " by " + session.username);
        return;
    }

    if (type == PacketType::GROUP_APPROVE_REQ) {
        ApproveJoinRequest req; req.deserialize(buf);
        groupManager.approveJoin(req.groupName, session.username, req.username);
        logger.log("APPROVE_JOIN " + req.groupName + " user " + req.username + " by " + session.username);
        return;
    }

    if (type == PacketType::GROUP_INVITE_REQ) {
        InviteUserRequest req; req.deserialize(buf);
        groupManager.inviteUser(req.groupName, session.username, req.username);
        logger.log("INVITE_USER " + req.username + " to " + req.groupName + " by " + session.username);
        return;
    }

    if (type == PacketType::GROUP_ACCEPT_INVITE_REQ) {
        AcceptInviteRequest req; req.deserialize(buf);
        groupManager.acceptInvite(req.groupName, session.username);
        logger.log("ACCEPT_INVITE " + req.groupName + " by " + session.username);
        return;
    }

    if (type == PacketType::GROUP_LEAVE_REQ) {
        LeaveGroupRequest req; req.deserialize(buf);
        groupManager.leaveGroup(req.groupName, session.username);
        logger.log("LEAVE_GROUP " + req.groupName + " by " + session.username);
        return;
    }

    if (type == PacketType::GROUP_KICK_REQ) {
        KickMemberRequest req; req.deserialize(buf);
        groupManager.kickMember(req.groupName, session.username, req.username);
        logger.log("KICK_MEMBER " + req.username + " from " + req.groupName + " by " + session.username);
        return;
    }

    if (type == PacketType::GROUP_LIST_MEMBERS_REQ) {
        ListMembersRequest req; req.deserialize(buf);
        auto members = groupManager.listMembers(req.groupName);
        ListMembersResponse res{members};

        ByteBuffer outBuf;
        res.serialize(outBuf);

        // Gửi header ở network order
        PacketHeader hdr_net{};
        hdr_net.type     = htons(static_cast<uint16_t>(PacketType::GROUP_LIST_MEMBERS_RES));
        hdr_net.length   = htonl(static_cast<uint32_t>(outBuf.size()));
        hdr_net.seq      = htonl(0);
        hdr_net.checksum = htonl(0);

        // DEBUG
        {
            unsigned char* p = reinterpret_cast<unsigned char*>(&hdr_net);
            std::cout << "[Server] raw header to send:";
            for (size_t i = 0; i < sizeof(hdr_net); ++i) printf(" %02x", p[i]);
            std::cout << "\n";
        }

        send(session.fd, &hdr_net, sizeof(hdr_net), 0);
        if (outBuf.size() > 0) send(session.fd, outBuf.data(), outBuf.size(), 0);
        logger.log("LIST_MEMBERS " + req.groupName + " by " + session.username);
        return;
    }

    if (type == PacketType::GROUP_LIST_REQ) {
        try {
            std::cout << "[Server] GROUP_LIST_REQ received\n"; // DEBUG
            ListGroupsResponse res;
            res.ownedGroups = groupManager.listGroupsOwnedByUser(session.username);
            res.joinedGroups = groupManager.listGroupsByUser(session.username);

            ByteBuffer outBuf;
            res.serialize(outBuf);

            PacketHeader hdr_net{};
            hdr_net.type     = htons(static_cast<uint16_t>(PacketType::GROUP_LIST_RES));
            hdr_net.length   = htonl(static_cast<uint32_t>(outBuf.size()));
            hdr_net.seq      = htonl(0);
            hdr_net.checksum = htonl(0);

            // DEBUG
            {
                unsigned char* p = reinterpret_cast<unsigned char*>(&hdr_net);
                std::cout << "[Server] raw header to send:";
                for (size_t i = 0; i < sizeof(hdr_net); ++i) printf(" %02x", p[i]);
                std::cout << "\n";
            }

            send(session.fd, &hdr_net, sizeof(hdr_net), 0);
            if (outBuf.size() > 0) send(session.fd, outBuf.data(), outBuf.size(), 0);

            logger.log("LIST_GROUPS for " + session.username);
        } catch (...) {
            std::cout << "[Server] unknown exception in GROUP_LIST_REQ\n";
            logger.log("ERR LIST_GROUP_REQ: unknown");
        }
        return;
    }

    if (type == PacketType::GROUP_PENDING_REQ) {
        PendingListResponse res;
        auto groups = groupManager.loadGroups();
        for (auto& [name,g] : groups) {
            for (auto& u : g.pending_join)
                res.joinRequests.push_back({name,u});
            for (auto& u : g.pending_invite)
                res.invites.push_back({name,u, g.owner});
        }
        ByteBuffer out; res.serialize(out);
        PacketHeader hdr_net{};
        hdr_net.type = htons((uint16_t)PacketType::GROUP_PENDING_RES);
        hdr_net.length = htonl((uint32_t)out.size());
        send(session.fd,&hdr_net,sizeof(hdr_net),0);
        if(out.size()>0) send(session.fd,out.data(),out.size(),0);
        return;
    }
    if (type == PacketType::GROUP_REJECT_JOIN_REQ) {
        RejectJoinRequest req; req.deserialize(buf);
        groupManager.rejectJoin(req.groupName, session.username, req.username);
        return;
    }
    if (type == PacketType::GROUP_REJECT_INVITE_REQ) {
        RejectInviteRequest req; req.deserialize(buf);
        groupManager.rejectInvite(req.groupName, session.username);
        return;
    }


    // ===== FILE SYSTEM =====
    if (type == PacketType::FILE_LIST_REQ) {
        FileListRequest req; req.deserialize(buf);
        FileListResponse resList{};
        if (permissionChecker.canPerform(session.username, req.groupName, FileAction::LIST)) {
            try {
                auto entries = fsManager.list(req.groupName, req.path);
                resList.entries.clear();
                for (const auto& e : entries) {
                    FileListEntry le;
                    le.name = e.name;
                    le.isDir = e.isDir;
                    le.size = e.size;
                    resList.entries.push_back(le);
                }
                ByteBuffer out;
                resList.serialize(out);

                PacketHeader hdr_net{};
                hdr_net.type     = htons(static_cast<uint16_t>(PacketType::FILE_LIST_RES));
                hdr_net.length   = htonl(static_cast<uint32_t>(out.size()));
                hdr_net.seq      = htonl(0);
                hdr_net.checksum = htonl(0);

                // DEBUG
                {
                    unsigned char* p = reinterpret_cast<unsigned char*>(&hdr_net);
                    std::cout << "[Server] raw header to send:";
                    for (size_t i = 0; i < sizeof(hdr_net); ++i) printf(" %02x", p[i]);
                    std::cout << "\n";
                }

                send(session.fd, &hdr_net, sizeof(hdr_net), 0);
                if (out.size() > 0) send(session.fd, out.data(), out.size(), 0);
                logger.log("LIST_FOLDER " + req.groupName + "/" + req.path + " by " + session.username);
            } catch (...) {
                // có thể gửi ACTION RESULT fail nếu muốn
            }
        }
        return;
    }

    // FILE_MKDIR_REQ
    if (type == PacketType::FILE_MKDIR_REQ) {
        MkdirRequest req; req.deserialize(buf);
        ActionResult ar{};
        if (permissionChecker.canPerform(session.username, req.groupName, FileAction::MKDIR)) {
            bool ok = false;
            try { ok = fsManager.makeDir(req.groupName, req.path); } catch (...) { ok = false; }
            ar.success = ok; ar.message = ok ? "mkdir ok" : "mkdir fail";
        } else {
            ar.success = false; ar.message = "permission denied";
        }
        ByteBuffer out; ar.serialize(out);

        PacketHeader hdr_net{};
        hdr_net.type     = htons(static_cast<uint16_t>(PacketType::FILE_MKDIR_RES));
        hdr_net.length   = htonl(static_cast<uint32_t>(out.size()));
        hdr_net.seq      = htonl(0);
        hdr_net.checksum = htonl(0);

        // DEBUG
        {
            unsigned char* p = reinterpret_cast<unsigned char*>(&hdr_net);
            std::cout << "[Server] raw header to send:";
            for (size_t i = 0; i < sizeof(hdr_net); ++i) printf(" %02x", p[i]);
            std::cout << "\n";
        }

        send(session.fd, &hdr_net, sizeof(hdr_net), 0);
        if (out.size() > 0) send(session.fd, out.data(), out.size(), 0);
        logger.log("MKDIR " + req.groupName + "/" + req.path + " by " + session.username);
        return;
    }

    // FILE_DELETE_REQ
    if (type == PacketType::FILE_DELETE_REQ) {
        DeleteRequest req; req.deserialize(buf);
        ActionResult ar{};
        if (permissionChecker.canPerform(session.username, req.groupName, FileAction::DELETE)) {
            bool ok = false;
            try { ok = fsManager.removePath(req.groupName, req.path); } catch (...) { ok = false; }
            ar.success = ok; ar.message = ok ? "delete ok" : "delete fail";
        } else {
            ar.success = false; ar.message = "permission denied";
        }
        ByteBuffer out; ar.serialize(out);

        PacketHeader hdr_net{};
        hdr_net.type     = htons(static_cast<uint16_t>(PacketType::FILE_DELETE_RES));
        hdr_net.length   = htonl(static_cast<uint32_t>(out.size()));
        hdr_net.seq      = htonl(0);
        hdr_net.checksum = htonl(0);

        // DEBUG
        {
            unsigned char* p = reinterpret_cast<unsigned char*>(&hdr_net);
            std::cout << "[Server] raw header to send:";
            for (size_t i = 0; i < sizeof(hdr_net); ++i) printf(" %02x", p[i]);
            std::cout << "\n";
        }

        send(session.fd, &hdr_net, sizeof(hdr_net), 0);
        if (out.size() > 0) send(session.fd, out.data(), out.size(), 0);
        logger.log("DELETE " + req.groupName + "/" + req.path + " by " + session.username);
        return;
    }

    // FILE_RENAME_REQ
    if (type == PacketType::FILE_RENAME_REQ) {
        RenameRequest req; req.deserialize(buf);
        ActionResult ar{};
        if (permissionChecker.canPerform(session.username, req.groupName, FileAction::RENAME)) {
            bool ok = false;
            try { ok = fsManager.renamePath(req.groupName, req.oldPath, req.newPath);} catch (...) { ok = false; }
            ar.success = ok; ar.message = ok ? "rename ok" : "rename fail";
        } else {
            ar.success = false; ar.message = "permission denied";
        }
        ByteBuffer out; ar.serialize(out);

        PacketHeader hdr_net{};
        hdr_net.type     = htons(static_cast<uint16_t>(PacketType::FILE_RENAME_RES));
        hdr_net.length   = htonl(static_cast<uint32_t>(out.size()));
        hdr_net.seq      = htonl(0);
        hdr_net.checksum = htonl(0);

        // DEBUG
        {
            unsigned char* p = reinterpret_cast<unsigned char*>(&hdr_net);
            std::cout << "[Server] raw header to send:";
            for (size_t i = 0; i < sizeof(hdr_net); ++i) printf(" %02x", p[i]);
            std::cout << "\n";
        }

        send(session.fd, &hdr_net, sizeof(hdr_net), 0);
        if (out.size() > 0) send(session.fd, out.data(), out.size(), 0);
        logger.log("RENAME " + req.groupName + "/" + req.oldPath + "->" + req.newPath + " by " + session.username);
        return;
    }


if (type == PacketType::FILE_COPY_REQ) {
    CopyRequest req; 
    req.deserialize(buf);

    ActionResult ar{};

    if (permissionChecker.canPerform(session.username, req.groupName, FileAction::COPY)) {
        try {
            bool ok = fsManager.copyPath(req.groupName, req.srcPath, req.dstPath);
            ar.success = ok;
            ar.message = ok ? "copy done" : "copy failed";
        } catch (const std::exception& e) {
            ar.success = false;
            ar.message = e.what();
        }
    } else {
        ar.success = false;
        ar.message = "permission denied";
    }

    ByteBuffer out;
    ar.serialize(out);

    PacketHeader hdr_net{};
    hdr_net.type     = htons((uint16_t)PacketType::FILE_COPY_RES);
    hdr_net.length   = htonl((uint32_t)out.size());
    hdr_net.seq      = htonl(0);
    hdr_net.checksum = htonl(0);

    send(session.fd, &hdr_net, sizeof(hdr_net), 0);
    if (out.size() > 0)
        send(session.fd, out.data(), out.size(), 0);

    return;
}

if (type == PacketType::FILE_MOVE_REQ) {
    MoveRequest req; 
    req.deserialize(buf);

    ActionResult ar{};

    if (permissionChecker.canPerform(session.username, req.groupName, FileAction::MOVE)) {
        try {
            bool ok = fsManager.movePath(req.groupName, req.srcPath, req.dstPath);
            ar.success = ok;
            ar.message = ok ? "move done" : "move failed";
        } catch (const std::exception& e) {
            ar.success = false;
            ar.message = e.what();
        }
    } else {
        ar.success = false;
        ar.message = "permission denied";
    }

    ByteBuffer out;
    ar.serialize(out);

    PacketHeader hdr_net{};
    hdr_net.type     = htons((uint16_t)PacketType::FILE_MOVE_RES);
    hdr_net.length   = htonl((uint32_t)out.size());
    hdr_net.seq      = htonl(0);
    hdr_net.checksum = htonl(0);

    send(session.fd, &hdr_net, sizeof(hdr_net), 0);
    if (out.size() > 0)
        send(session.fd, out.data(), out.size(), 0);

    return;
}


    // ===== FILE UPLOAD (streaming) =====
    if (type == PacketType::FILE_UPLOAD_BEGIN) {
        UploadBegin req; req.deserialize(buf);
        ActionResult ar{};

        if (!permissionChecker.canPerform(session.username, req.groupName, FileAction::UPLOAD)) {
            ar.success = false;
            ar.message = "permission denied";
        } else {
            try {
                std::filesystem::path dst =
                fsManager.resolvePath(req.groupName, req.remotePath);
                            fs::create_directories(fs::path(dst).parent_path());

                uploadStreams[session.fd].open(dst, std::ios::binary | std::ios::trunc);
                if (!uploadStreams[session.fd]) {
                    ar.success = false;
                    ar.message = "cannot open file";
                } else {
                    uploadRemaining[session.fd] = req.totalSize;
                    ar.success = true;
                    ar.message = "upload begin ok";
                }
            } catch (...) {
                ar.success = false;
                ar.message = "upload begin fail";
            }
        }

        ByteBuffer out; ar.serialize(out);
        PacketHeader hdr_net{};
        hdr_net.type     = htons((uint16_t)PacketType::FILE_UPLOAD_RES);
        hdr_net.length   = htonl((uint32_t)out.size());
        hdr_net.seq      = htonl(0);
        hdr_net.checksum = htonl(0);

        send(session.fd, &hdr_net, sizeof(hdr_net), 0);
        if (out.size() > 0)
            send(session.fd, out.data(), out.size(), 0);
        return;
    }

    if (type == PacketType::FILE_UPLOAD_CHUNK) {
        UploadChunk req; req.deserialize(buf);

        if (uploadStreams[session.fd].is_open() && !req.data.empty()) {
            uploadStreams[session.fd].write(
                reinterpret_cast<const char*>(req.data.data()),
                req.data.size()
            );
            uploadRemaining[session.fd] -= req.data.size();
        }
        return;
    }

    if (type == PacketType::FILE_UPLOAD_END) {
    ActionResult ar{};

    if (!uploadStreams[session.fd].is_open()) {
        ar.success = false;
        ar.message = "no upload stream";
    } else {
        uploadStreams[session.fd].close();
        bool ok = (uploadRemaining[session.fd] == 0);
        ar.success = ok;
        ar.message = ok ? "upload done" : "upload size mismatch";
        uploadRemaining[session.fd] = 0;
    }

    ByteBuffer out; ar.serialize(out);
    PacketHeader hdr_net{};
    hdr_net.type     = htons((uint16_t)PacketType::FILE_UPLOAD_RES);
    hdr_net.length   = htonl((uint32_t)out.size());
    hdr_net.seq      = htonl(0);
    hdr_net.checksum = htonl(0);

    send(session.fd, &hdr_net, sizeof(hdr_net), 0);
    if (out.size() > 0)
        send(session.fd, out.data(), out.size(), 0);
    logger.log("UPLOAD_END by " + session.username);
    return;
}
    // ===== FILE DOWNLOAD (streaming) =====
    if (type == PacketType::FILE_DOWNLOAD_BEGIN) {
        DownloadBegin req; req.deserialize(buf);
        ActionResult ar{};
        if (!permissionChecker.canPerform(session.username, req.groupName, FileAction::DOWNLOAD)) {
            ar.success = false; ar.message = "permission denied";
        } else {
            try {
                std::filesystem::path src =
                fsManager.resolvePath(req.groupName, req.remotePath);
                std::ifstream ifs(src, std::ios::binary);
                const size_t chunkSize = 64 * 1024;
                std::vector<uint8_t> bufChunk;
                while (ifs.good()) {
                    bufChunk.resize(chunkSize);
                    ifs.read(reinterpret_cast<char*>(bufChunk.data()), chunkSize);
                    std::streamsize got = ifs.gcount();
                    if (got <= 0) break;
                    bufChunk.resize(static_cast<size_t>(got));
                    DownloadChunk chunk{bufChunk};
                    ByteBuffer out;
                    chunk.serialize(out);

                    PacketHeader hdr_net{};
                    hdr_net.type     = htons(static_cast<uint16_t>(PacketType::FILE_DOWNLOAD_CHUNK));
                    hdr_net.length   = htonl(static_cast<uint32_t>(out.size()));
                    hdr_net.seq      = htonl(0);
                    hdr_net.checksum = htonl(0);

                    // DEBUG
                    {
                        unsigned char* p = reinterpret_cast<unsigned char*>(&hdr_net);
                        std::cout << "[Server] raw header to send:";
                        for (size_t i = 0; i < sizeof(hdr_net); ++i) printf(" %02x", p[i]);
                        std::cout << "\n";
                    }

                    send(session.fd, &hdr_net, sizeof(hdr_net), 0);
                    if (out.size() > 0) send(session.fd, out.data(), out.size(), 0);
                }

                DownloadEnd endMsg;
                ByteBuffer endBuf; endMsg.serialize(endBuf);

                PacketHeader endHdr{};
                endHdr.type     = htons(static_cast<uint16_t>(PacketType::FILE_DOWNLOAD_END));
                endHdr.length   = htonl(static_cast<uint32_t>(endBuf.size()));
                endHdr.seq      = htonl(0);
                endHdr.checksum = htonl(0);

                // DEBUG
                {
                    unsigned char* p = reinterpret_cast<unsigned char*>(&endHdr);
                    std::cout << "[Server] raw header to send:";
                    for (size_t i = 0; i < sizeof(endHdr); ++i) printf(" %02x", p[i]);
                    std::cout << "\n";
                }

                send(session.fd, &endHdr, sizeof(endHdr), 0);
                if (endBuf.size() > 0) send(session.fd, endBuf.data(), endBuf.size(), 0);

                ar.success = true; ar.message = "download done";
            } catch (...) {
                ar.success = false; ar.message = "download fail";
            }
        }

        ByteBuffer out; ar.serialize(out);

        PacketHeader hdr_net{};
        hdr_net.type     = htons(static_cast<uint16_t>(PacketType::FILE_DOWNLOAD_RES));
        hdr_net.length   = htonl(static_cast<uint32_t>(out.size()));
        hdr_net.seq      = htonl(0);
        hdr_net.checksum = htonl(0);

        // DEBUG
        {
            unsigned char* p = reinterpret_cast<unsigned char*>(&hdr_net);
            std::cout << "[Server] raw header to send:";
            for (size_t i = 0; i < sizeof(hdr_net); ++i) printf(" %02x", p[i]);
            std::cout << "\n";
        }

        send(session.fd, &hdr_net, sizeof(hdr_net), 0);
        if (out.size() > 0) send(session.fd, out.data(), out.size(), 0);
        logger.log("DOWNLOAD_BEGIN by " + session.username);
        return;
    }

    // ===== UNKNOWN =====
    std::cout << "Unknown packet type\n";
}
