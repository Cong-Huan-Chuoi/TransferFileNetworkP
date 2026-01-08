#include "server/Server.h"
#include "protocol/packets.h"
#include <string>
#include <sys/socket.h>

void Server::handle_packet(int fd,
                           const PacketHeader& header,
                           const std::vector<uint8_t>& payload) {
    ClientSession* sess = sessionManager.get_session(fd);
    std::string data(payload.begin(), payload.end());

    switch (header.type) {

    /* ========= AUTH ========= */

    case PacketType::REGISTER: {
        auto pos = data.find(':');
        if (pos == std::string::npos) {
            send_error(fd, "format: user:pass");
            break;
        }
        bool ok = authManager.register_user(
            data.substr(0, pos),
            data.substr(pos + 1)
        );
        ok ? send_ok(fd) : send_error(fd, "user exists");
        break;
    }

    case PacketType::LOGIN: {
        auto pos = data.find(':');
        if (pos == std::string::npos) {
            send_error(fd, "format: user:pass");
            break;
        }
        bool ok = authManager.login_user(
            fd,
            data.substr(0, pos),
            data.substr(pos + 1),
            sessionManager
        );
        ok ? send_ok(fd) : send_error(fd, "login failed");
        break;
    }

    /* ========= GROUP ========= */

    case PacketType::CREATE_GROUP: {
        if (!sess->logged_in) {
            send_error(fd, "not logged in");
            break;
        }
        bool ok = groupManager.create_group(data, sess->username);
        if (ok) {
            fsManager.create_group_root(data);
            send_ok(fd);
        } else {
            send_error(fd, "group exists");
        }
        break;
    }

    case PacketType::LIST_GROUPS: {
        auto groups = groupManager.list_groups();
        std::string out;
        for (auto& g : groups) out += g + "\n";
        send_ok(fd, out);
        break;
    }

    case PacketType::JOIN_GROUP_REQUEST: {
        bool ok = groupManager.request_join(data, sess->username);
        ok ? send_ok(fd) : send_error(fd, "join failed");
        break;
    }

    case PacketType::APPROVE_JOIN: {
        bool ok = groupManager.approve_join_request(
            sess->current_group,
            sess->username,
            data
        );
        ok ? send_ok(fd) : send_error(fd, "approve failed");
        break;
    }

    /* ========= FILE UPLOAD ========= */

    case PacketType::UPLOAD_REQUEST: {
        if (!permissionChecker.can_upload(
                sess->current_group, sess->username)) {
            send_error(fd, "no permission");
            break;
        }
        fileReceiver.start_upload(
            fd,
            sess->current_group,
            data,
            header.payload_len
        );
        send_ok(fd);
        break;
    }

    case PacketType::UPLOAD_CHUNK: {
        fileReceiver.receive_chunk(fd,
            payload.data(), payload.size());
        break;
    }

    case PacketType::UPLOAD_FINISH: {
        fileReceiver.finish_upload(fd);
        send_ok(fd);
        break;
    }
        /* ========= FILE / DIR ========= */

    case PacketType::LIST_DIR: {
        if (!permissionChecker.can_download(
                sess->current_group, sess->username)) {
            send_error(fd, "permission denied");
            break;
        }

        auto items = fsManager.list_dir(
            sess->current_group, data);

        std::string out;
        for (const auto& it : items)
            out += it + "\n";

        send_ok(fd, out);
        logger.info(sess->username + " list_dir " + data);
        break;
    }

    case PacketType::MKDIR: {
        if (!permissionChecker.can_create_dir(
                sess->current_group, sess->username)) {
            send_error(fd, "permission denied");
            break;
        }

        bool ok = fsManager.create_dir(
            sess->current_group, data);

        ok ? send_ok(fd) : send_error(fd, "mkdir failed");
        logger.info(sess->username + " mkdir " + data);
        break;
    }

    case PacketType::RENAME: {
        if (!permissionChecker.can_rename_file(
                sess->current_group, sess->username)) {
            send_error(fd, "permission denied");
            break;
        }

        auto pos = data.find('|');
        if (pos == std::string::npos) {
            send_error(fd, "format old|new");
            break;
        }

        std::string oldp = data.substr(0, pos);
        std::string newp = data.substr(pos + 1);

        bool ok =
            fsManager.rename_file(sess->current_group, oldp, newp) ||
            fsManager.rename_dir(sess->current_group, oldp, newp);

        ok ? send_ok(fd) : send_error(fd, "rename failed");
        logger.info(sess->username + " rename " + oldp + " -> " + newp);
        break;
    }

    case PacketType::DELETE_: {
        bool ok = false;

        // thử xóa file (chỉ owner)
        if (permissionChecker.can_delete_file(
                sess->current_group, sess->username)) {
            ok = fsManager.remove_file(
                sess->current_group, data);
        }

        // nếu không phải file → thử dir (owner)
        if (!ok &&
            permissionChecker.can_delete_dir(
                sess->current_group, sess->username)) {
            ok = fsManager.remove_dir(
                sess->current_group, data);
        }

        ok ? send_ok(fd) : send_error(fd, "delete failed");
        logger.info(sess->username + " delete " + data);
        break;
    }

    /* ========= DOWNLOAD ========= */

    case PacketType::DOWNLOAD_REQUEST: {
        if (!permissionChecker.can_download(
                sess->current_group, sess->username)) {
            send_error(fd, "permission denied");
            break;
        }

        constexpr size_t CHUNK_SIZE = 4096;
        size_t offset = 0;
        std::vector<uint8_t> chunk;

        while (true) {
            chunk.clear();

            bool ok = fsManager.read_file_chunk(
                sess->current_group,
                data,
                offset,
                CHUNK_SIZE,
                chunk);

            if (!ok || chunk.empty())
                break;

            auto pkt = make_packet(
                PacketType::DOWNLOAD_CHUNK,
                FLAG_NONE,
                static_cast<uint32_t>(offset),
                chunk);

            send(fd, pkt.data(), pkt.size(), 0);
            offset += chunk.size();
        }

        auto fin = make_packet(
            PacketType::DOWNLOAD_FINISH,
            FLAG_LAST,
            0,
            {});

        send(fd, fin.data(), fin.size(), 0);
        logger.info(sess->username + " download " + data);
        break;
    }

    default:
        send_error(fd, "unknown packet");
    }
}
