#pragma once
#include <string>
#include <vector>

struct ActionResult {
    bool ok;
    std::string message;
};

class GroupManager;

class GroupService {
public:
    explicit GroupService(GroupManager& gm);

    // ===== BASIC GROUP =====
    void createGroup(const std::string& group,
                     const std::string& owner);

    void requestJoin(const std::string& group,
                     const std::string& username);

    ActionResult approveJoin(
        const std::string& group,
        const std::string& leader,
        const std::string& username);

    ActionResult rejectJoin(
        const std::string& group,
        const std::string& leader,
        const std::string& username);

    void inviteUser(const std::string& group,
                    const std::string& leader,
                    const std::string& username);

    void leaveGroup(const std::string& group,
                    const std::string& username);

    void kickUser(const std::string& group,
                  const std::string& leader,
                  const std::string& username);

    // ===== QUERY =====
    std::vector<std::string> listMembers(const std::string& group,
                                         const std::string& requester);

    std::vector<std::string> listOwnedGroups(const std::string& user);
    std::vector<std::string> listJoinedGroups(const std::string& user);

private:
    GroupManager& groupManager;
};
