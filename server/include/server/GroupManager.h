#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Metadata của 1 nhóm
struct Group {
    std::string name;
    std::string owner;

    std::unordered_set<std::string> members;
    std::unordered_set<std::string> pending_requests;
    std::unordered_set<std::string> invited_users;
};

/*
GroupManager:
    quản lý metadata nhóm
    lưu vào file groups.db
*/

class GroupManager{
    public:
        explicit GroupManager(const std::string& db_path);

        bool create_group(const std::string& group_name,
                          const std::string& owner);
        
        bool request_join(const std::string& group_name,
                          const std::string& username);

        bool invite_user(const std::string& group_name,
                         const std::string& owner,
                         const std::string& username);
        
        bool approve_join_request(const std::string& group_name,
                                  const std::string& owner,
                                  const std::string& username);

        bool accept_invite(const std::string& group_name,
                           const std::string& username);

        bool leave_group(const std::string& group_name,
                         const std::string& username);

        bool remove_member(const std::string& group_name,
                           const std::string& owner,
                           const std::string& username);

        //query

        std::vector<std::string> list_groups() const;
        std::vector<std::string> list_members(const std::string& group_name) const;

        bool is_member(const std::string& group_name,
                       const std::string& username) const;

        bool is_owner(const std::string& group_name,
                      const std::string& username) const;

    private:
            void load_db();
            void save_db() const;

            static std::unordered_set<std::string> split(const std::string& s);
            static std::string join(const std::unordered_set<std::string>& s);
    private:
            std::string db_path;
            std::unordered_map<std::string, Group> groups;
};