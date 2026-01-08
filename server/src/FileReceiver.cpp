#include "server/FileReceiver.h"

FileReceiver::FileReceiver(FileSystemManager& fs)
    : fsManager(fs) {}

bool FileReceiver::start_upload(int fd,
                                const std::string& group,
                                const std::string& path,
                                size_t total_size) {
    if (uploads.count(fd))
        return false;

    UploadContext ctx;
    ctx.group = group;
    ctx.path = path;
    ctx.received = 0;
    ctx.total = total_size;

    uploads[fd] = ctx;

    // tạo file rỗng
    fsManager.write_file_chunk(group, path, nullptr, 0, false);
    return true;
}

bool FileReceiver::receive_chunk(int fd,
                                 const uint8_t* data,
                                 size_t size) {
    auto it = uploads.find(fd);
    if (it == uploads.end())
        return false;

    UploadContext& ctx = it->second;

    fsManager.write_file_chunk(ctx.group,
                               ctx.path,
                               data,
                               size,
                               true);

    ctx.received += size;
    return true;
}

bool FileReceiver::finish_upload(int fd) {
    auto it = uploads.find(fd);
    if (it == uploads.end())
        return false;

    uploads.erase(it);
    return true;
}

bool FileReceiver::is_uploading(int fd) const {
    return uploads.count(fd);
}
