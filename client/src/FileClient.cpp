#include "client/FileClient.h"
#include "client/Client.h"

#include <fstream>

FileClient::FileClient(Client& c) : client(c) {}

bool FileClient::handle(const std::string& cmd) {
    if (cmd.starts_with("upload ")) {
        upload(cmd.substr(7));
        return true;
    }

    if (cmd.starts_with("download ")) {
        download(cmd.substr(9));
        return true;
    }

    return false;
}

void FileClient::upload(const std::string& path) {
    std::ifstream ifs(path, std::ios::binary);
    if (!ifs.is_open()) return;

    client.send_packet(PacketType::UPLOAD_REQUEST, path);

    char buf[4096];
    while (ifs.read(buf, sizeof(buf)) || ifs.gcount()) {
        client.send_packet(
            PacketType::UPLOAD_CHUNK,
            std::string(buf, ifs.gcount())
        );
    }

    client.send_packet(PacketType::UPLOAD_FINISH, "");
}

void FileClient::download(const std::string& path) {
    client.send_packet(PacketType::DOWNLOAD_REQUEST, path);
}
