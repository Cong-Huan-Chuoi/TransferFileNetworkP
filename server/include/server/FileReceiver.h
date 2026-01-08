#pragma once

#include <string>
#include <unordered_map>
#include <cstdint>

#include "server/FileSystemManager.h"

/*
 * Thông tin một upload đang diễn ra
 */
struct UploadContext {
    std::string group;
    std::string path;
    size_t received;
    size_t total;
};

/*
 * FileReceiver
 *  - Quản lý trạng thái upload
 *  - Ghi file theo chunk
 */
class FileReceiver {
public:
    explicit FileReceiver(FileSystemManager& fs);

    bool start_upload(int fd,
                      const std::string& group,
                      const std::string& path,
                      size_t total_size);

    bool receive_chunk(int fd,
                       const uint8_t* data,
                       size_t size);

    bool finish_upload(int fd);

    bool is_uploading(int fd) const;

private:
    FileSystemManager& fsManager;
    std::unordered_map<int, UploadContext> uploads;
};
