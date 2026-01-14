#pragma once
#include <string>
#include <vector>
#include <cstdint>

class FileSystemManager {
public:
    struct Entry {
        std::string name;
        bool isDir;
        uint64_t size; // = 0 nếu là thư mục
    };

    explicit FileSystemManager(const std::string& baseDir);

    // ===== Core folder operations =====
    std::vector<Entry> list(const std::string& group,
                            const std::string& path);

    bool makeDir(const std::string& group,
                 const std::string& path);

    // ===== Common file/folder operations =====
    bool removePath(const std::string& group,
                    const std::string& path);

    bool renamePath(const std::string& group,
                    const std::string& oldPath,
                    const std::string& newPath);

    bool copyPath(const std::string& group,
                  const std::string& src,
                  const std::string& dst);

    bool movePath(const std::string& group,
                  const std::string& src,
                  const std::string& dst);

    // ===== Security helper =====
    std::string resolvePath(const std::string& group,
                            const std::string& path);

private:
    std::string base;
};
