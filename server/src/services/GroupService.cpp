#include "server/services/GroupService.h"
#include "server/GroupManager.h"

GroupService::GroupService(GroupManager& gm)
    : groupManager(gm) {}

void GroupService::createGroup(const std::string& owner,
                               const std::string& group) {
    groupManager.createGroup(group, owner);
}

void GroupService::requestJoin(const std::string& user,
                               const std::string& group) {
    groupManager.requestJoin(group, user);
}

void GroupService::approveJoin(const std::string& leader,
                               const std::string& user,
                               const std::string& group) {
    groupManager.approveJoin(group, leader, user);
}

void GroupService::inviteUser(const std::string& leader,
                              const std::string& user,
                              const std::string& group) {
    groupManager.inviteUser(group, leader, user);
}

void GroupService::leaveGroup(const std::string& user,
                              const std::string& group) {
    groupManager.leaveGroup(group, user);
}

void GroupService::kickUser(const std::string& leader,
                            const std::string& user,
                            const std::string& group) {
    groupManager.kickMember(group, leader, user);
}

std::vector<std::string>
GroupService::listMembers(const std::string& requester,
                          const std::string& group) {
    return groupManager.listMembers(group);
}

std::vector<std::string>
GroupService::listJoinedGroups(const std::string& user) {
    return groupManager.listGroupsByUser(user);
}

std::vector<std::string>
GroupService::listOwnedGroups(const std::string& user) {
    return groupManager.listGroupsOwnedByUser(user);
}
