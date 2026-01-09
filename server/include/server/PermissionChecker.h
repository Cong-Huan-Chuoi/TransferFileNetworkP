#pragma once
#include <string>
#include "server/GroupManager.h"

enum class FileAction {
    LIST,
    UPLOAD,
    DOWNLOAD,
    MKDIR,
    RENAME,
    DELETE,
    MOVE,
    COPY
};

class PermissionChecker {
public:
    explicit PermissionChecker(GroupManager& gm);

    bool canPerform(const std::string& user,
                    const std::string& group,
                    FileAction action);

private:
    GroupManager& groupManager;
};
