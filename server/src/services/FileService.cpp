#include "server/services/FileService.h"
#include "server/FileSystemManager.h"
#include "server/PermissionChecker.h"

FileService::FileService(FileSystemManager& fs,
                         PermissionChecker& perm)
    : fsManager(fs), permissionChecker(perm) {}

std::vector<std::string>
FileService::list(const std::string& user,
                  const std::string& group,
                  const std::string& path) {
    if (!permissionChecker.canPerform(
            user, group, FileAction::LIST)) {
        return {};
    }
    return fsManager.list(group, path);
}

void FileService::mkdir(const std::string& user,
                        const std::string& group,
                        const std::string& path) {
    if (permissionChecker.canPerform(
            user, group, FileAction::MKDIR)) {
        fsManager.makeDir(group, path);   // ĐÚNG TÊN HÀM
    }
}

void FileService::remove(const std::string& user,
                         const std::string& group,
                         const std::string& path) {
    if (permissionChecker.canPerform(
            user, group, FileAction::DELETE)) {
        fsManager.remove(group, path);
    }
}
