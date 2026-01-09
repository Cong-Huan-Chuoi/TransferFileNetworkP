#include "server/PermissionChecker.h"

PermissionChecker::PermissionChecker(GroupManager& gm)
    : groupManager(gm) {}

bool PermissionChecker::canPerform(const std::string& user,
                                   const std::string& group,
                                   FileAction action) {
    auto members = groupManager.listMembers(group);
    if (members.empty()) return false;

    bool isMember = false;
    for (auto& m : members)
        if (m == user) isMember = true;

    if (!isMember) return false;

    // owner = first member
    bool isOwner = (members.front() == user);

    switch (action) {
    case FileAction::LIST:
    case FileAction::UPLOAD:
    case FileAction::DOWNLOAD:
    case FileAction::MKDIR:
        return true;

    case FileAction::RENAME:
    case FileAction::DELETE:
    case FileAction::MOVE:
    case FileAction::COPY:
        return isOwner;
    }
    return false;
}
    