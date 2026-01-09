#pragma once
#include <string>
#include <vector>

class FileSystemManager;
class PermissionChecker;

enum class FileAction;

class FileService {
public:
    FileService(FileSystemManager& fs,
                PermissionChecker& perm);

    std::vector<std::string>
    list(const std::string& user,
         const std::string& group,
         const std::string& path);

    void mkdir(const std::string& user,
               const std::string& group,
               const std::string& path);

    void remove(const std::string& user,
                const std::string& group,
                const std::string& path);

private:
    FileSystemManager& fsManager;
    PermissionChecker& permissionChecker;
};
