#include "server/GroupManager.h"
#include <fstream>
#include <sstream>

/* ===================== Utility ===================== */

std::unordered_set<std::string>
GroupManager::split(const std::string& s) {
    std::unordered_set<std::string> result;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, ',')) {
        if (!item.empty())
            result.insert(item);
    }
    return result;
}

std::string
GroupManager::join(const std::unordered_set<std::string>& s) {
    std::string out;
    bool first = true;
    for (const auto& x : s) {
        if (!first) out += ",";
        out += x;
        first = false;
    }
    return out;
}

/* ===================== Constructor ===================== */

GroupManager::GroupManager(const std::string& path)
    : db_path(path) {
    load_db();
}

/* ===================== DB ===================== */

void GroupManager::load_db() {
    std::ifstream file(db_path);
    if (!file.is_open())
        return;

    std::string line;
    Group g;

    while (std::getline(file, line)) {
        if (line == "[group]") {
            if (!g.name.empty())
                groups[g.name] = g;
            g = Group{};
        } else if (line.rfind("name=", 0) == 0) {
            g.name = line.substr(5);
        } else if (line.rfind("owner=", 0) == 0) {
            g.owner = line.substr(6);
        } else if (line.rfind("members=", 0) == 0) {
            g.members = split(line.substr(8));
        } else if (line.rfind("pending=", 0) == 0) {
            g.pending_requests = split(line.substr(8));
        } else if (line.rfind("invited=", 0) == 0) {
            g.invited_users = split(line.substr(8));
        }
    }

    if (!g.name.empty())
        groups[g.name] = g;
}

void GroupManager::save_db() const {
    std::ofstream file(db_path, std::ios::trunc);

    for (const auto& [_, g] : groups) {
        file << "[group]\n";
        file << "name=" << g.name << "\n";
        file << "owner=" << g.owner << "\n";
        file << "members=" << join(g.members) << "\n";
        file << "pending=" << join(g.pending_requests) << "\n";
        file << "invited=" << join(g.invited_users) << "\n\n";
    }
}

/* ===================== Logic ===================== */

bool GroupManager::create_group(const std::string& group_name,
                                const std::string& owner) {
    if (groups.count(group_name))
        return false;

    Group g;
    g.name = group_name;
    g.owner = owner;
    g.members.insert(owner);

    groups[group_name] = std::move(g);
    save_db();
    return true;
}

bool GroupManager::request_join(const std::string& group_name,
                                const std::string& username) {
    auto it = groups.find(group_name);
    if (it == groups.end())
        return false;

    Group& g = it->second;
    if (g.members.count(username))
        return false;

    g.pending_requests.insert(username);
    save_db();
    return true;
}

bool GroupManager::invite_user(const std::string& group_name,
                               const std::string& owner,
                               const std::string& username) {
    auto it = groups.find(group_name);
    if (it == groups.end())
        return false;

    Group& g = it->second;
    if (g.owner != owner)
        return false;

    g.invited_users.insert(username);
    save_db();
    return true;
}

bool GroupManager::approve_join_request(const std::string& group_name,
                                        const std::string& owner,
                                        const std::string& username) {
    auto it = groups.find(group_name);
    if (it == groups.end())
        return false;

    Group& g = it->second;
    if (g.owner != owner)
        return false;

    if (!g.pending_requests.erase(username))
        return false;

    g.members.insert(username);
    save_db();
    return true;
}

bool GroupManager::accept_invite(const std::string& group_name,
                                 const std::string& username) {
    auto it = groups.find(group_name);
    if (it == groups.end())
        return false;

    Group& g = it->second;
    if (!g.invited_users.erase(username))
        return false;

    g.members.insert(username);
    save_db();
    return true;
}

bool GroupManager::leave_group(const std::string& group_name,
                               const std::string& username) {
    auto it = groups.find(group_name);
    if (it == groups.end())
        return false;

    Group& g = it->second;
    if (g.owner == username)
        return false;

    bool ok = g.members.erase(username);
    if (ok) save_db();
    return ok;
}

bool GroupManager::remove_member(const std::string& group_name,
                                 const std::string& owner,
                                 const std::string& username) {
    auto it = groups.find(group_name);
    if (it == groups.end())
        return false;

    Group& g = it->second;
    if (g.owner != owner)
        return false;

    bool ok = g.members.erase(username);
    if (ok) save_db();
    return ok;
}

/* ===================== Query ===================== */

std::vector<std::string> GroupManager::list_groups() const {
    std::vector<std::string> res;
    for (const auto& [name, _] : groups)
        res.push_back(name);
    return res;
}

std::vector<std::string>
GroupManager::list_members(const std::string& group_name) const {
    std::vector<std::string> res;
    auto it = groups.find(group_name);
    if (it == groups.end())
        return res;

    for (const auto& u : it->second.members)
        res.push_back(u);
    return res;
}

bool GroupManager::is_member(const std::string& group_name,
                             const std::string& username) const {
    auto it = groups.find(group_name);
    if (it == groups.end())
        return false;
    return it->second.members.count(username);
}

bool GroupManager::is_owner(const std::string& group_name,
                            const std::string& username) const {
    auto it = groups.find(group_name);
    if (it == groups.end())
        return false;
    return it->second.owner == username;
}
