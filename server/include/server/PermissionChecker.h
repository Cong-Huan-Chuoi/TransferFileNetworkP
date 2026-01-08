#pragma once

#include <string>

#include "server/SessionManager.h"
#include "server/GroupManager.h"

/*
 * PermissionChecker
 *  - Kiểm tra quyền truy cập
 *  - KHÔNG thao tác file
 *  - KHÔNG gửi packet
 */
class PermissionChecker {
public:
    PermissionChecker(const SessionManager& sessionMgr,
                      const GroupManager& groupMgr);

    bool can_upload(const std::string& group,
                    const std::string& username) const;

    bool can_download(const std::string& group,
                      const std::string& username) const;

    bool can_create_dir(const std::string& group,
                        const std::string& username) const;

    bool can_delete_file(const std::string& group,
                         const std::string& username) const;

    bool can_rename_file(const std::string& group,
                         const std::string& username) const;

    bool can_delete_dir(const std::string& group,
                        const std::string& username) const;

    bool can_rename_dir(const std::string& group,
                        const std::string& username) const;

private:
    const SessionManager& sessionManager;
    const GroupManager& groupManager;
};
