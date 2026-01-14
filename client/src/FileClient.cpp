#include "client/FileClient.h"
#include "protocol/packets.h"
#include "protocol/packet_types.h"
#include <iostream>
#include <fstream>
#include <vector>

FileClient::FileClient(Client& c) : client(c) {}

// ================= LIST =================
void FileClient::listFolder() {
    std::string path;
    std::cout << "Path (\".\"): ";
    std::cin >> path;

    auto& session = client.getSession();
    FileListRequest req{session.current_group, path};

    ByteBuffer buf; 
    req.serialize(buf);
    client.sendPacket((uint16_t)PacketType::FILE_LIST_REQ, buf);

    PacketHeader hdr; 
    ByteBuffer payload;
    if (client.recvPacket(hdr, payload) &&
        hdr.type == (uint16_t)PacketType::FILE_LIST_RES) {

        FileListResponse res; 
        res.deserialize(payload);
        std::cout << "Entries:\n";
        for (const auto& e : res.entries) {
            if (e.isDir)
                std::cout << " [DIR]  " << e.name << "\n";
            else
                std::cout << " [FILE] " << e.name
                          << " (" << e.size << " bytes)\n";
        }
    } else {
        std::cout << "List failed\n";
    }
}

// ================= MKDIR =================
void FileClient::createFolder() {
    std::string path;
    std::cout << "New folder path: ";
    std::cin >> path;

    auto& session = client.getSession();
    MkdirRequest req{session.current_group, path};

    ByteBuffer buf; 
    req.serialize(buf);
    client.sendPacket((uint16_t)PacketType::FILE_MKDIR_REQ, buf);

    PacketHeader hdr; 
    ByteBuffer payload;
    if (client.recvPacket(hdr, payload) &&
        hdr.type == (uint16_t)PacketType::FILE_MKDIR_RES) {

        ActionResult ar; 
        ar.deserialize(payload);
        std::cout << (ar.success ? "OK: " : "ERR: ")
                  << ar.message << "\n";
    } else {
        std::cout << "Mkdir failed\n";
    }
}

// ================= DELETE =================
void FileClient::deleteEntry() {
    std::string path;
    std::cout << "Entry to delete: ";
    std::cin >> path;

    auto& session = client.getSession();
    DeleteRequest req{session.current_group, path};

    ByteBuffer buf; 
    req.serialize(buf);
    client.sendPacket((uint16_t)PacketType::FILE_DELETE_REQ, buf);

    PacketHeader hdr; 
    ByteBuffer payload;
    if (client.recvPacket(hdr, payload) &&
        hdr.type == (uint16_t)PacketType::FILE_DELETE_RES) {

        ActionResult ar; 
        ar.deserialize(payload);
        std::cout << (ar.success ? "OK: " : "ERR: ")
                  << ar.message << "\n";
    } else {
        std::cout << "Delete failed\n";
    }
}

// ================= RENAME =================
void FileClient::renameEntry() {
    std::string oldp, newp;
    std::cout << "Old path: ";
    std::cin >> oldp;
    std::cout << "New path: ";
    std::cin >> newp;

    auto& session = client.getSession();
    RenameRequest req{session.current_group, oldp, newp};

    ByteBuffer buf; 
    req.serialize(buf);
    client.sendPacket((uint16_t)PacketType::FILE_RENAME_REQ, buf);

    PacketHeader hdr; 
    ByteBuffer payload;
    if (client.recvPacket(hdr, payload) &&
        hdr.type == (uint16_t)PacketType::FILE_RENAME_RES) {

        ActionResult ar; 
        ar.deserialize(payload);
        std::cout << (ar.success ? "OK: " : "ERR: ")
                  << ar.message << "\n";
    } else {
        std::cout << "Rename failed\n";
    }
}

// ================= COPY =================
void FileClient::copyEntry() {
    std::string src, dst;
    std::cout << "Src path: ";
    std::cin >> src;
    std::cout << "Dst path: ";
    std::cin >> dst;

    auto& session = client.getSession();
    CopyRequest req{session.current_group, src, dst};

    ByteBuffer buf; 
    req.serialize(buf);
    client.sendPacket((uint16_t)PacketType::FILE_COPY_REQ, buf);

    PacketHeader hdr; 
    ByteBuffer payload;
    if (client.recvPacket(hdr, payload) &&
        hdr.type == (uint16_t)PacketType::FILE_COPY_RES) {

        ActionResult ar; 
        ar.deserialize(payload);
        std::cout << (ar.success ? "OK: " : "ERR: ")
                  << ar.message << "\n";
    } else {
        std::cout << "Copy failed\n";
    }
}

// ================= MOVE =================
void FileClient::moveEntry() {
    std::string src, dst;
    std::cout << "Src path: ";
    std::cin >> src;
    std::cout << "Dst path: ";
    std::cin >> dst;

    auto& session = client.getSession();
    MoveRequest req{session.current_group, src, dst};

    ByteBuffer buf; 
    req.serialize(buf);
    client.sendPacket((uint16_t)PacketType::FILE_MOVE_REQ, buf);

    PacketHeader hdr; 
    ByteBuffer payload;
    if (client.recvPacket(hdr, payload) &&
        hdr.type == (uint16_t)PacketType::FILE_MOVE_RES) {

        ActionResult ar; 
        ar.deserialize(payload);
        std::cout << (ar.success ? "OK: " : "ERR: ")
                  << ar.message << "\n";
    } else {
        std::cout << "Move failed\n";
    }
}

// ================= UPLOAD  =================
void FileClient::uploadFile() {
    std::string remote, local;
    std::cout << "Remote path: ";
    std::cin >> remote;
    std::cout << "Local file: ";
    std::cin >> local;

    std::ifstream ifs(local, std::ios::binary | std::ios::ate);
    if (!ifs) {
        std::cout << "Cannot open local file\n";
        return;
    }

    uint64_t total = static_cast<uint64_t>(ifs.tellg());
    ifs.seekg(0, std::ios::beg);

    auto& session = client.getSession();
    UploadBegin begin{session.current_group, remote, total};
    ByteBuffer bbuf; 
    begin.serialize(bbuf);
    client.sendPacket((uint16_t)PacketType::FILE_UPLOAD_BEGIN, bbuf);

    PacketHeader hdr; 
    ByteBuffer payload;
    if (!client.recvPacket(hdr, payload) ||
        hdr.type != (uint16_t)PacketType::FILE_UPLOAD_RES) {
        std::cout << "Upload rejected\n";
        return;
    }

    ActionResult ar; 
    ar.deserialize(payload);
    if (!ar.success) {
        std::cout << "ERR: " << ar.message << "\n";
        return;
    }

    const size_t chunkSize = 64 * 1024;
    std::vector<uint8_t> chunk(chunkSize);

    while (ifs.good()) {
        ifs.read(reinterpret_cast<char*>(chunk.data()), chunkSize);
        std::streamsize got = ifs.gcount();
        if (got <= 0) break;

        UploadChunk c;
        c.data.assign(chunk.begin(), chunk.begin() + got);
        ByteBuffer cbuf; 
        c.serialize(cbuf);
        client.sendPacket((uint16_t)PacketType::FILE_UPLOAD_CHUNK, cbuf);
    }

    UploadEnd end;
    ByteBuffer ebuf; 
    end.serialize(ebuf);
    client.sendPacket((uint16_t)PacketType::FILE_UPLOAD_END, ebuf);

    if (client.recvPacket(hdr, payload) &&
        hdr.type == (uint16_t)PacketType::FILE_UPLOAD_RES) {
        ar.deserialize(payload);
        std::cout << (ar.success ? "OK: " : "ERR: ")
                  << ar.message << "\n";
    }
}

// ================= DOWNLOAD =================
void FileClient::downloadFile() {
    std::string remote, local;
    std::cout << "Remote path: ";
    std::cin >> remote;
    std::cout << "Local save to: ";
    std::cin >> local;

    auto& session = client.getSession();
    DownloadBegin req{session.current_group, remote};

    ByteBuffer buf; 
    req.serialize(buf);
    client.sendPacket((uint16_t)PacketType::FILE_DOWNLOAD_BEGIN, buf);

    std::ofstream ofs(local, std::ios::binary | std::ios::trunc);
    if (!ofs) {
        std::cout << "Cannot open local dest\n";
        return;
    }

    while (true) {
        PacketHeader hdr; 
        ByteBuffer payload;
        if (!client.recvPacket(hdr, payload)) {
            std::cout << "Download read error\n";
            break;
        }

        if (hdr.type == (uint16_t)PacketType::FILE_DOWNLOAD_CHUNK) {
            DownloadChunk c; 
            c.deserialize(payload);
            if (!c.data.empty())
                ofs.write(reinterpret_cast<const char*>(c.data.data()),
                          c.data.size());
        }
        else if (hdr.type == (uint16_t)PacketType::FILE_DOWNLOAD_END) {
            PacketHeader rh; 
            ByteBuffer rp;
            if (client.recvPacket(rh, rp) &&
                rh.type == (uint16_t)PacketType::FILE_DOWNLOAD_RES) {
                ActionResult ar; 
                ar.deserialize(rp);
                std::cout << (ar.success ? "OK: " : "ERR: ")
                          << ar.message << "\n";
            }
            break;
        }
    }
}
