#include "server/FileSystemManager.h"
#include <filesystem>

namespace fs = std::filesystem;

FileSystemManager::FileSystemManager(const std::string& baseDir)
    : base(baseDir) {}

std::string FileSystemManager::resolvePath(const std::string& group,
                                           const std::string& path) {
    fs::path p = fs::path(base) / group / path;
    p = fs::weakly_canonical(p);

    fs::path root = fs::weakly_canonical(fs::path(base) / group);
    if (p.string().find(root.string()) != 0)
        throw std::runtime_error("Path traversal");

    return p.string();
}

std::vector<FileSystemManager::Entry>
FileSystemManager::list(const std::string& group,
                        const std::string& path) {
    std::vector<Entry> res;
    auto full = resolvePath(group, path);

    for (auto& e : fs::directory_iterator(full)) {
        Entry ent;
        ent.name = e.path().filename().string();
        ent.isDir = e.is_directory();
        ent.size = ent.isDir ? 0 : fs::file_size(e);
        res.push_back(ent);
    }
    return res;
}

bool FileSystemManager::makeDir(const std::string& group,
                                const std::string& path) {
    auto full = resolvePath(group, path);
    return fs::create_directories(full);
}

bool FileSystemManager::removePath(const std::string& group,
                                   const std::string& path) {
    auto full = resolvePath(group, path);
    return fs::remove_all(full) > 0;
}

bool FileSystemManager::renamePath(const std::string& group,
                                   const std::string& oldPath,
                                   const std::string& newPath) {
    auto src = resolvePath(group, oldPath);
    auto dst = resolvePath(group, newPath);
    fs::rename(src, dst);
    return true;
}

bool FileSystemManager::copyPath(const std::string& group,
                                 const std::string& src,
                                 const std::string& dst) {
    fs::copy(resolvePath(group, src),
             resolvePath(group, dst),
             fs::copy_options::recursive);
    return true;
}

bool FileSystemManager::movePath(const std::string& group,
                                 const std::string& src,
                                 const std::string& dst) {
    fs::rename(resolvePath(group, src),
               resolvePath(group, dst));
    return true;
}
