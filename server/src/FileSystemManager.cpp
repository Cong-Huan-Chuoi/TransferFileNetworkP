#include "server/FileSystemManager.h"
#include <fstream>

/* ================= Constructor ================= */

FileSystemManager::FileSystemManager(const std::string& base_dir)
    : base(base_dir) {
    std::filesystem::create_directories(base);
}

/* ================= Path resolve ================= */

std::filesystem::path
FileSystemManager::resolve_path(const std::string& group,
                                const std::string& relative_path) const {
    std::filesystem::path p = base / group / relative_path;
    return std::filesystem::weakly_canonical(p);
}

/* ================= Directory ================= */

bool FileSystemManager::create_group_root(const std::string& group) {
    return std::filesystem::create_directories(base / group);
}

bool FileSystemManager::create_dir(const std::string& group,
                                   const std::string& relative_path) {
    return std::filesystem::create_directories(
        resolve_path(group, relative_path));
}

bool FileSystemManager::remove_dir(const std::string& group,
                                   const std::string& relative_path) {
    return std::filesystem::remove_all(
        resolve_path(group, relative_path)) > 0;
}

bool FileSystemManager::rename_dir(const std::string& group,
                                   const std::string& old_path,
                                   const std::string& new_path) {
    std::filesystem::rename(
        resolve_path(group, old_path),
        resolve_path(group, new_path));
    return true;
}

/* ================= File ================= */

bool FileSystemManager::remove_file(const std::string& group,
                                    const std::string& relative_path) {
    return std::filesystem::remove(
        resolve_path(group, relative_path));
}

bool FileSystemManager::rename_file(const std::string& group,
                                    const std::string& old_path,
                                    const std::string& new_path) {
    std::filesystem::rename(
        resolve_path(group, old_path),
        resolve_path(group, new_path));
    return true;
}

bool FileSystemManager::copy_file(const std::string& group,
                                  const std::string& src,
                                  const std::string& dst) {
    std::filesystem::copy_file(
        resolve_path(group, src),
        resolve_path(group, dst),
        std::filesystem::copy_options::overwrite_existing);
    return true;
}

bool FileSystemManager::move_file(const std::string& group,
                                  const std::string& src,
                                  const std::string& dst) {
    std::filesystem::rename(
        resolve_path(group, src),
        resolve_path(group, dst));
    return true;
}

/* ================= Listing ================= */

std::vector<std::string>
FileSystemManager::list_dir(const std::string& group,
                            const std::string& relative_path) const {
    std::vector<std::string> result;
    auto dir = resolve_path(group, relative_path);

    if (!std::filesystem::exists(dir))
        return result;

    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        result.push_back(entry.path().filename().string());
    }
    return result;
}

/* ================= Upload / Download ================= */

bool FileSystemManager::write_file_chunk(const std::string& group,
                                         const std::string& relative_path,
                                         const uint8_t* data,
                                         size_t size,
                                         bool append) {
    auto path = resolve_path(group, relative_path);

    std::ofstream ofs;
    if (append)
        ofs.open(path, std::ios::binary | std::ios::app);
    else
        ofs.open(path, std::ios::binary | std::ios::trunc);

    if (!ofs.is_open())
        return false;

    ofs.write(reinterpret_cast<const char*>(data), size);
    return true;
}

bool FileSystemManager::read_file_chunk(const std::string& group,
                                        const std::string& relative_path,
                                        size_t offset,
                                        size_t size,
                                        std::vector<uint8_t>& out) const {
    auto path = resolve_path(group, relative_path);

    std::ifstream ifs(path, std::ios::binary);
    if (!ifs.is_open())
        return false;

    ifs.seekg(offset);
    out.resize(size);
    ifs.read(reinterpret_cast<char*>(out.data()), size);

    out.resize(ifs.gcount());
    return true;
}
