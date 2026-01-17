#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <filesystem> 

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

    bool copyPath(const std::string& groupName,
                const std::string& srcPath,
                const std::string& dstPath);

    bool movePath(const std::string& groupName,
                const std::string& srcPath,
                const std::string& dstPath);

    // ===== Security helper =====
        std::filesystem::path resolvePath(
        const std::string& group,
        const std::string& path
    );

private:
    std::string base;
};
