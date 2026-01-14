#include "server/GroupManager.h"

#include <fstream>
#include <sstream>
#include <algorithm>



GroupManager::GroupManager(const std::string& path)
    : dbPath(path) {}

bool GroupManager::contains(const std::vector<std::string>& v,
                            const std::string& x) {
    for (auto& s : v) if (s == x) return true;
    return false;
}

void GroupManager::remove(std::vector<std::string>& v,
                          const std::string& x) {
    v.erase(std::remove(v.begin(), v.end(), x), v.end());
}

std::unordered_map<std::string, Group> GroupManager::loadGroups() {
    std::unordered_map<std::string, Group> groups;
    std::ifstream in(dbPath);
    if (!in.is_open()) return groups;

    std::string line;
    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;

        Group g;
        std::istringstream iss(line);
        std::string token;

        std::getline(iss, g.name, '|');

        while (std::getline(iss, token, '|')) {
            auto pos = token.find('=');
            if (pos == std::string::npos) continue;

            std::string key = token.substr(0, pos);
            std::string val = token.substr(pos + 1);

            std::stringstream ss(val);
            std::string item;

            if (key == "owner") g.owner = val;
            else if (key == "members") {
                while (std::getline(ss, item, ','))
                    if (!item.empty()) g.members.push_back(item);
            }
            else if (key == "pending_join") {
                while (std::getline(ss, item, ','))
                    if (!item.empty()) g.pending_join.push_back(item);
            }
            else if (key == "pending_invite") {
                while (std::getline(ss, item, ','))
                    if (!item.empty()) g.pending_invite.push_back(item);
            }
        }
        groups[g.name] = g;
    }
    return groups;
}

void GroupManager::saveGroups(
    const std::unordered_map<std::string, Group>& groups) {

    std::ofstream out(dbPath, std::ios::trunc);
    for (auto& [name, g] : groups) {
        out << name
            << "|owner=" << g.owner
            << "|members=";

        for (size_t i = 0; i < g.members.size(); ++i) {
            if (i) out << ",";
            out << g.members[i];
        }

        out << "|pending_join=";
        for (size_t i = 0; i < g.pending_join.size(); ++i) {
            if (i) out << ",";
            out << g.pending_join[i];
        }

        out << "|pending_invite=";
        for (size_t i = 0; i < g.pending_invite.size(); ++i) {
            if (i) out << ",";
            out << g.pending_invite[i];
        }

        out << "\n";
    }
}

// GroupManager.cpp (sửa createGroup)
bool GroupManager::createGroup(const std::string& groupName, const std::string& owner) {
  std::lock_guard<std::mutex> lock(dbMutex);
  auto groups = loadGroups();
  if (groups.count(groupName)) return false;

  Group g;
  g.name = groupName;
  g.owner = owner;
  g.members.push_back(owner);
  groups[groupName] = g;
  saveGroups(groups);

  // tạo thư mục nhóm (an toàn: defer logic sang server nơi có baseDir)
  // -> phần này mình sẽ thực hiện trong Server.cpp sau khi createGroup trả true

  return true;
}


std::vector<std::string> GroupManager::listGroups() {
    std::lock_guard<std::mutex> lock(dbMutex);
    auto groups = loadGroups();
    std::vector<std::string> res;
    for (auto& [name, _] : groups)
        res.push_back(name);
    return res;
}

bool GroupManager::requestJoin(const std::string& groupName,
                               const std::string& username) {
    std::lock_guard<std::mutex> lock(dbMutex);
    auto groups = loadGroups();
    auto& g = groups[groupName];

    if (contains(g.members, username) ||
        contains(g.pending_join, username))
        return false;

    g.pending_join.push_back(username);
    saveGroups(groups);
    return true;
}

bool GroupManager::approveJoin(const std::string& groupName,
                               const std::string& owner,
                               const std::string& username) {
    std::lock_guard<std::mutex> lock(dbMutex);
    auto groups = loadGroups();
    auto& g = groups[groupName];

    if (g.owner != owner) return false;
    if (!contains(g.pending_join, username)) return false;

    remove(g.pending_join, username);

    if (!contains(g.members, username)) { 
        g.members.push_back(username); 
    }
    saveGroups(groups);
    return true;
}

bool GroupManager::inviteUser(const std::string& groupName,
                              const std::string& owner,
                              const std::string& username) {
    std::lock_guard<std::mutex> lock(dbMutex);
    auto groups = loadGroups();
    auto& g = groups[groupName];

    if (g.owner != owner) return false;
    if (contains(g.members, username) ||
        contains(g.pending_invite, username))
        return false;

    g.pending_invite.push_back(username);
    saveGroups(groups);
    return true;
}

bool GroupManager::acceptInvite(const std::string& groupName,
                                const std::string& username) {
    std::lock_guard<std::mutex> lock(dbMutex);
    auto groups = loadGroups();
    auto& g = groups[groupName];

    if (!contains(g.pending_invite, username)) return false;

    remove(g.pending_invite, username);
    if (!contains(g.members, username)){
        g.members.push_back(username);
    }
    saveGroups(groups);
    return true;
}

bool GroupManager::leaveGroup(const std::string& groupName,
                              const std::string& username) {
    std::lock_guard<std::mutex> lock(dbMutex);
    auto groups = loadGroups();
    auto& g = groups[groupName];

    if (username == g.owner) return false;
    if (!contains(g.members, username)) return false;

    remove(g.members, username);
    saveGroups(groups);
    return true;
}

bool GroupManager::kickMember(const std::string& groupName,
                              const std::string& owner,
                              const std::string& username) {
    std::lock_guard<std::mutex> lock(dbMutex);
    auto groups = loadGroups();
    auto& g = groups[groupName];

    if (g.owner != owner) return false;
    if (!contains(g.members, username)) return false;

    remove(g.members, username);
    saveGroups(groups);
    return true;
}

bool GroupManager::rejectJoin(const std::string& groupName,
                              const std::string& owner,
                              const std::string& username) {
    std::lock_guard<std::mutex> lock(dbMutex);
    auto groups = loadGroups();
    auto& g = groups[groupName];
    if (g.owner != owner) return false;
    if (!contains(g.pending_join, username)) return false;
    remove(g.pending_join, username);
    saveGroups(groups);
    return true;
}

bool GroupManager::rejectInvite(const std::string& groupName,
                                const std::string& username) {
    std::lock_guard<std::mutex> lock(dbMutex);
    auto groups = loadGroups();
    auto& g = groups[groupName];
    if (!contains(g.pending_invite, username)) return false;
    remove(g.pending_invite, username);
    saveGroups(groups);
    return true;
}


std::vector<std::string> GroupManager::listMembers(const std::string& groupName) {
    std::lock_guard<std::mutex> lock(dbMutex);
    auto groups = loadGroups();
    auto it = groups.find(groupName);
    if (it == groups.end()) return {};
    return it->second.members;
}


std::vector<std::string> GroupManager::listGroupsByUser(const std::string& username) {
    std::lock_guard<std::mutex> lock(dbMutex);
    auto groups = loadGroups();
    std::vector<std::string> res;
    for (auto& [name, g] : groups) {
        if (contains(g.members, username)) {
            res.push_back(name);
        }
    }
    return res;
}

std::vector<std::string> GroupManager::listGroupsOwnedByUser(const std::string& username) {
    std::lock_guard<std::mutex> lock(dbMutex);
    auto groups = loadGroups();
    std::vector<std::string> res;
    for (auto& [name, g] : groups) {
        if (g.owner == username) {
            res.push_back(name);
        }
    }
    return res;
}

