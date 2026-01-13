#pragma once
#include <string>
#include <vector>

class FileSystemManager {
public:
    explicit FileSystemManager(const std::string& baseDir);

    std::vector<std::string> list(const std::string& group,
                                  const std::string& path);

    bool makeDir(const std::string& group,
                 const std::string& path);

    bool remove(const std::string& group,
                const std::string& path);

    bool rename(const std::string& group,
                const std::string& oldPath,
                const std::string& newPath);

    std::string resolvePath(const std::string& group,
                            const std::string& path);

private:
    std::string base;
};
    