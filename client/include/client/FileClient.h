// FileClient.h (sửa/mở rộng)
#pragma once
#include "Client.h"
#include <string>

class FileClient {
public:
  explicit FileClient(Client& c);

  void listFolder();        // FILE_LIST_REQ/RES
  void createFolder();      // FILE_MKDIR_REQ/RES
  void uploadFile();        // FILE_UPLOAD_BEGIN/CHUNK/END + FILE_UPLOAD_RES
  void downloadFile();      // FILE_DOWNLOAD_BEGIN + recv CHUNK/END + FILE_DOWNLOAD_RES
  void deleteEntry();       // FILE_DELETE_REQ/RES (owner)
  void renameEntry();       // FILE_RENAME_REQ/RES (owner)
  void copyEntry();         // FILE_COPY_REQ/RES (owner)
  void moveEntry();         // FILE_MOVE_REQ/RES (owner)

private:
  Client& client;
};
