// FileClient.cpp (sửa/mở rộng)
#include "client/FileClient.h"
#include "protocol/packets.h"
#include "protocol/packet_types.h"
#include <iostream>
#include <fstream>
#include <vector>

FileClient::FileClient(Client& c) : client(c) {}

void FileClient::listFolder() {
  std::string group, path;
  std::cout << "Group: "; std::cin >> group;
  std::cout << "Path (\".\"): "; std::cin >> path;

  FileListRequest req{group, path};
  ByteBuffer buf; req.serialize(buf);
  client.sendPacket((uint16_t)PacketType::FILE_LIST_REQ, buf);

  PacketHeader hdr; ByteBuffer payload;
  if (client.recvPacket(hdr, payload) && hdr.type == (uint16_t)PacketType::FILE_LIST_RES) {
    FileListResponse res; res.deserialize(payload);
    std::cout << "Entries:\n";
    for (auto& e : res.entries) std::cout << " - " << e << "\n";
  } else {
    std::cout << "List failed\n";
  }
}

void FileClient::createFolder() {
  std::string group, path;
  std::cout << "Group: "; std::cin >> group;
  std::cout << "New folder path: "; std::cin >> path;

  MkdirRequest req{group, path};
  ByteBuffer buf; req.serialize(buf);
  client.sendPacket((uint16_t)PacketType::FILE_MKDIR_REQ, buf);

  PacketHeader hdr; ByteBuffer payload;
  if (client.recvPacket(hdr, payload) && hdr.type == (uint16_t)PacketType::FILE_MKDIR_RES) {
    ActionResult ar; ar.deserialize(payload);
    std::cout << (ar.success ? "OK: " : "ERR: ") << ar.message << "\n";
  } else {
    std::cout << "Mkdir failed\n";
  }
}

void FileClient::deleteEntry() {
  std::string group, path;
  std::cout << "Group: "; std::cin >> group;
  std::cout << "Entry to delete: "; std::cin >> path;

  DeleteRequest req{group, path};
  ByteBuffer buf; req.serialize(buf);
  client.sendPacket((uint16_t)PacketType::FILE_DELETE_REQ, buf);

  PacketHeader hdr; ByteBuffer payload;
  if (client.recvPacket(hdr, payload) && hdr.type == (uint16_t)PacketType::FILE_DELETE_RES) {
    ActionResult ar; ar.deserialize(payload);
    std::cout << (ar.success ? "OK: " : "ERR: ") << ar.message << "\n";
  } else {
    std::cout << "Delete failed\n";
  }
}

void FileClient::renameEntry() {
  std::string group, oldp, newp;
  std::cout << "Group: "; std::cin >> group;
  std::cout << "Old path: "; std::cin >> oldp;
  std::cout << "New path: "; std::cin >> newp;

  RenameRequest req{group, oldp, newp};
  ByteBuffer buf; req.serialize(buf);
  client.sendPacket((uint16_t)PacketType::FILE_RENAME_REQ, buf);

  PacketHeader hdr; ByteBuffer payload;
  if (client.recvPacket(hdr, payload) && hdr.type == (uint16_t)PacketType::FILE_RENAME_RES) {
    ActionResult ar; ar.deserialize(payload);
    std::cout << (ar.success ? "OK: " : "ERR: ") << ar.message << "\n";
  } else {
    std::cout << "Rename failed\n";
  }
}

void FileClient::copyEntry() {
  std::string group, src, dst;
  std::cout << "Group: "; std::cin >> group;
  std::cout << "Src path: "; std::cin >> src;
  std::cout << "Dst path: "; std::cin >> dst;

  CopyRequest req{group, src, dst};
  ByteBuffer buf; req.serialize(buf);
  client.sendPacket((uint16_t)PacketType::FILE_COPY_REQ, buf);

  PacketHeader hdr; ByteBuffer payload;
  if (client.recvPacket(hdr, payload) && hdr.type == (uint16_t)PacketType::FILE_COPY_RES) {
    ActionResult ar; ar.deserialize(payload);
    std::cout << (ar.success ? "OK: " : "ERR: ") << ar.message << "\n";
  } else {
    std::cout << "Copy failed\n";
  }
}

void FileClient::moveEntry() {
  std::string group, src, dst;
  std::cout << "Group: "; std::cin >> group;
  std::cout << "Src path: "; std::cin >> src;
  std::cout << "Dst path: "; std::cin >> dst;

  MoveRequest req{group, src, dst};
  ByteBuffer buf; req.serialize(buf);
  client.sendPacket((uint16_t)PacketType::FILE_MOVE_REQ, buf);

  PacketHeader hdr; ByteBuffer payload;
  if (client.recvPacket(hdr, payload) && hdr.type == (uint16_t)PacketType::FILE_MOVE_RES) {
    ActionResult ar; ar.deserialize(payload);
    std::cout << (ar.success ? "OK: " : "ERR: ") << ar.message << "\n";
  } else {
    std::cout << "Move failed\n";
  }
}

void FileClient::uploadFile() {
  std::string group, remote, local;
  std::cout << "Group: "; std::cin >> group;
  std::cout << "Remote path: "; std::cin >> remote;
  std::cout << "Local file: "; std::cin >> local;

  std::ifstream ifs(local, std::ios::binary | std::ios::ate);
  if (!ifs) { std::cout << "Cannot open local file\n"; return; }
  auto total = static_cast<uint64_t>(ifs.tellg());
  ifs.seekg(0, std::ios::beg);

  UploadBegin begin{group, remote, total};
  ByteBuffer bbuf; begin.serialize(bbuf);
  client.sendPacket((uint16_t)PacketType::FILE_UPLOAD_BEGIN, bbuf);

  const size_t chunkSize = 64 * 1024;
  std::vector<uint8_t> chunk(chunkSize);
  while (ifs.good()) {
    ifs.read(reinterpret_cast<char*>(chunk.data()), chunkSize);
    std::streamsize got = ifs.gcount();
    if (got <= 0) break;
    UploadChunk c;
    c.data.assign(chunk.begin(), chunk.begin() + static_cast<size_t>(got));
    ByteBuffer cbuf; c.serialize(cbuf);
    client.sendPacket((uint16_t)PacketType::FILE_UPLOAD_CHUNK, cbuf);
  }

  UploadEnd end;
  ByteBuffer ebuf; end.serialize(ebuf);
  client.sendPacket((uint16_t)PacketType::FILE_UPLOAD_END, ebuf);

  PacketHeader hdr; ByteBuffer payload;
  if (client.recvPacket(hdr, payload) && hdr.type == (uint16_t)PacketType::FILE_UPLOAD_RES) {
    ActionResult ar; ar.deserialize(payload);
    std::cout << (ar.success ? "OK: " : "ERR: ") << ar.message << "\n";
  } else {
    std::cout << "Upload failed\n";
  }
}

void FileClient::downloadFile() {
  std::string group, remote, local;
  std::cout << "Group: "; std::cin >> group;
  std::cout << "Remote path: "; std::cin >> remote;
  std::cout << "Local save to: "; std::cin >> local;

  DownloadBegin req{group, remote};
  ByteBuffer buf; req.serialize(buf);
  client.sendPacket((uint16_t)PacketType::FILE_DOWNLOAD_BEGIN, buf);

  std::ofstream ofs(local, std::ios::binary | std::ios::trunc);
  if (!ofs) { std::cout << "Cannot open local dest\n"; return; }

  // read chunks until END
  while (true) {
    PacketHeader hdr; ByteBuffer payload;
    if (!client.recvPacket(hdr, payload)) { std::cout << "Download read error\n"; break; }
    if (hdr.type == (uint16_t)PacketType::FILE_DOWNLOAD_CHUNK) {
      DownloadChunk c; c.deserialize(payload);
      if (!c.data.empty()) {
        ofs.write(reinterpret_cast<const char*>(c.data.data()), c.data.size());
      }
    } else if (hdr.type == (uint16_t)PacketType::FILE_DOWNLOAD_END) {
      // wait final RES message
      PacketHeader rh; ByteBuffer rp;
      if (client.recvPacket(rh, rp) && rh.type == (uint16_t)PacketType::FILE_DOWNLOAD_RES) {
        ActionResult ar; ar.deserialize(rp);
        std::cout << (ar.success ? "OK: " : "ERR: ") << ar.message << "\n";
      } else {
        std::cout << "Download completion missing\n";
      }
      break;
    } else {
      // ignore other packets
    }
  }
}
