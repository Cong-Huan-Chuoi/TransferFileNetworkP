#include "server/PermissionChecker.h"

/*
 * Constructor
 */
PermissionChecker::PermissionChecker(const SessionManager& sessionMgr,
                                     const GroupManager& groupMgr)
    : sessionManager(sessionMgr),
      groupManager(groupMgr) {}

/*
 * Thành viên + owner đều được upload
 */
bool PermissionChecker::can_upload(const std::string& group,
                                   const std::string& username) const {
    return groupManager.is_member(group, username);
}

/*
 * Thành viên + owner đều được download
 */
bool PermissionChecker::can_download(const std::string& group,
                                     const std::string& username) const {
    return groupManager.is_member(group, username);
}

/*
 * Thành viên + owner đều được tạo thư mục
 */
bool PermissionChecker::can_create_dir(const std::string& group,
                                       const std::string& username) const {
    return groupManager.is_member(group, username);
}

/*
 * Chỉ owner được xóa file
 */
bool PermissionChecker::can_delete_file(const std::string& group,
                                        const std::string& username) const {
    return groupManager.is_owner(group, username);
}

/*
 * Chỉ owner được đổi tên file
 */
bool PermissionChecker::can_rename_file(const std::string& group,
                                        const std::string& username) const {
    return groupManager.is_owner(group, username);
}

/*
 * Chỉ owner được xóa thư mục
 */
bool PermissionChecker::can_delete_dir(const std::string& group,
                                       const std::string& username) const {
    return groupManager.is_owner(group, username);
}

/*
 * Chỉ owner được đổi tên thư mục
 */
bool PermissionChecker::can_rename_dir(const std::string& group,
                                       const std::string& username) const {
    return groupManager.is_owner(group, username);
}
