#pragma once
#include <string>
#include <vector>

class GroupManager;

class GroupService {
public:
    explicit GroupService(GroupManager& gm);

    // member
    void createGroup(const std::string& owner,
                     const std::string& group);

    void requestJoin(const std::string& user,
                     const std::string& group);

    void leaveGroup(const std::string& user,
                    const std::string& group);

    // leader
    void approveJoin(const std::string& leader,
                     const std::string& user,
                     const std::string& group);

    void inviteUser(const std::string& leader,
                    const std::string& user,
                    const std::string& group);

    void kickUser(const std::string& leader,
                  const std::string& user,
                  const std::string& group);

    // query
    std::vector<std::string>
    listMembers(const std::string& requester,
                const std::string& group);

    std::vector<std::string>
    listJoinedGroups(const std::string& user);

    std::vector<std::string>
    listOwnedGroups(const std::string& user);

private:
    GroupManager& groupManager;
};
