#include "server/PermissionChecker.h"
#include "server/GroupManager.h"

PermissionChecker::PermissionChecker(GroupManager& gm)
    : groupManager(gm) {}

bool PermissionChecker::canPerform(const std::string& user,
                                   const std::string& group,
                                   FileAction action)
{
    // ===== 1. Kiểm tra group tồn tại =====
    auto groups = groupManager.loadGroups();
    auto it = groups.find(group);
    if (it == groups.end()) {
        return false; // group không tồn tại
    }

    // ===== 2. Kiểm tra user có phải member =====
    const auto& members = groupManager.listMembers(group);
    bool isMember = false;
    for (const auto& m : members) {
        if (m == user) {
            isMember = true;
            break;
        }
    }
    if (!isMember) {
        return false; // không phải member thì cấm hết
    }

    // ===== 3. Kiểm tra owner =====
    bool isOwner = (it->second.owner == user);

    // ===== 4. Phân quyền theo action =====
    switch (action) {
        // member + owner
        case FileAction::LIST:
        case FileAction::UPLOAD:
        case FileAction::DOWNLOAD:
        case FileAction::MKDIR:
            return true;

        // chỉ owner
        case FileAction::RENAME:
        case FileAction::DELETE:
        case FileAction::MOVE:
        case FileAction::COPY:
            return isOwner;
    }

    return false;
}
