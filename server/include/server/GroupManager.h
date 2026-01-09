#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>

struct Group {
    std::string name;
    std::string owner;
    std::vector<std::string> members;
    std::vector<std::string> pending_join;
    std::vector<std::string> pending_invite;
};

class GroupManager {
public:
    explicit GroupManager(const std::string& dbPath);

    bool createGroup(const std::string& groupName,
                     const std::string& owner);

    std::vector<std::string> listGroups();

    bool requestJoin(const std::string& groupName,
                     const std::string& username);

    bool approveJoin(const std::string& groupName,
                     const std::string& owner,
                     const std::string& username);

    bool inviteUser(const std::string& groupName,
                    const std::string& owner,
                    const std::string& username);

    bool acceptInvite(const std::string& groupName,
                      const std::string& username);

    bool leaveGroup(const std::string& groupName,
                    const std::string& username);

    bool kickMember(const std::string& groupName,
                    const std::string& owner,
                    const std::string& username);

    std::vector<std::string> listMembers(const std::string& groupName);
    std::vector<std::string> listGroupsByUser(const std::string& username);
    std::vector<std::string> listGroupsOwnedByUser(const std::string& username);


private:
    std::string dbPath;
    std::mutex dbMutex;

    std::unordered_map<std::string, Group> loadGroups();
    void saveGroups(const std::unordered_map<std::string, Group>& groups);

    bool contains(const std::vector<std::string>& v,
                  const std::string& x);
    void remove(std::vector<std::string>& v,
                const std::string& x);
};
