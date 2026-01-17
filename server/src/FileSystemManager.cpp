#include "server/FileSystemManager.h"
#include <filesystem>

namespace fs = std::filesystem;

FileSystemManager::FileSystemManager(const std::string& baseDir)
    : base(baseDir) {}

std::filesystem::path FileSystemManager::resolvePath(
    const std::string& group,
    const std::string& path)
{
    fs::path p = fs::path(base) / group / path;
    p = fs::weakly_canonical(p);

    fs::path root = fs::weakly_canonical(fs::path(base) / group);
    if (p.string().find(root.string()) != 0)
        throw std::runtime_error("Path traversal");

    return p;
}

std::vector<FileSystemManager::Entry>
FileSystemManager::list(const std::string& group,
                        const std::string& path) {
    std::vector<Entry> res;
    auto full = resolvePath(group, path);
    if (!fs::exists(full)) { // Trả về rỗng, client sẽ in "group chưa có folder nào" 
        return res; 
    }
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

        bool FileSystemManager::copyPath(
        const std::string& group,
        const std::string& src,
        const std::string& dst)
    {
        namespace fs = std::filesystem;

        fs::path srcPath = resolvePath(group, src);
        fs::path dstPath = resolvePath(group, dst);

        if (!fs::exists(srcPath))
            throw std::runtime_error("source not exist");

        if (fs::exists(dstPath))
            throw std::runtime_error("destination exists");

        fs::create_directories(dstPath.parent_path());

        if (fs::is_directory(srcPath)) {
            fs::copy(srcPath, dstPath, fs::copy_options::recursive);
        } else {
            fs::copy_file(srcPath, dstPath);
        }
        return true;
    }


        bool FileSystemManager::movePath(
        const std::string& group,
        const std::string& src,
        const std::string& dst)
    {
        fs::path srcPath = resolvePath(group, src);
        fs::path dstPath = fs::path(base) / group / dst;

        if (!fs::exists(srcPath)) {
            return false;
        }

        fs::create_directories(dstPath.parent_path());
        dstPath = fs::weakly_canonical(dstPath);

        try {
            fs::rename(srcPath, dstPath);
            return true;
        } catch (...) {
            // fallback
            try {
                if (fs::is_directory(srcPath)) {
                    fs::copy(srcPath, dstPath,
                        fs::copy_options::recursive |
                        fs::copy_options::overwrite_existing);
                    fs::remove_all(srcPath);
                } else {
                    fs::copy_file(srcPath, dstPath,
                        fs::copy_options::overwrite_existing);
                    fs::remove(srcPath);
                }
                return true;
            } catch (...) {
                return false;
            }
        }
    }


