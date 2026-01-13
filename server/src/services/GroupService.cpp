#include "server/services/GroupService.h"
#include "server/GroupManager.h"

GroupService::GroupService(GroupManager& gm)
    : groupManager(gm) {}

// ===== BASIC GROUP =====

void GroupService::createGroup(const std::string& group,
                               const std::string& owner) {
    groupManager.createGroup(group, owner);
}

void GroupService::requestJoin(const std::string& group,
                               const std::string& username) {
    groupManager.requestJoin(group, username);
}

ActionResult GroupService::approveJoin(
    const std::string& group,
    const std::string& leader,
    const std::string& username
) {
    bool ok = groupManager.approveJoin(group, leader, username);
    if (!ok) {
        return {false, "Approve failed (not owner or no pending request)"};
    }
    return {true, "Approved join request"};
}

ActionResult GroupService::rejectJoin(
    const std::string& group,
    const std::string& leader,
    const std::string& username
) {
    return {false, "Reject join not implemented yet"};
}

void GroupService::inviteUser(const std::string& group,
                              const std::string& leader,
                              const std::string& username) {
    groupManager.inviteUser(group, leader, username);
}

void GroupService::leaveGroup(const std::string& group,
                              const std::string& username) {
    groupManager.leaveGroup(group, username);
}

void GroupService::kickUser(const std::string& group,
                            const std::string& leader,
                            const std::string& username) {
    groupManager.kickMember(group, leader, username);
}

// ===== QUERY =====

std::vector<std::string>
GroupService::listMembers(const std::string& group,
                          const std::string&) {
    return groupManager.listMembers(group);
}

std::vector<std::string>
GroupService::listOwnedGroups(const std::string& user) {
    return groupManager.listGroupsOwnedByUser(user);
}

std::vector<std::string>
GroupService::listJoinedGroups(const std::string& user) {
    return groupManager.listGroupsByUser(user);
}
