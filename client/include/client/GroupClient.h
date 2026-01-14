#pragma once
#include "Client.h"
#include <string>
#include <vector>
#include "protocol/packets.h"
struct GroupListCache {
    std::vector<std::string> owned;
    std::vector<std::string> joined;
};

class GroupClient {
public:
    explicit GroupClient(Client& c);

    // CRUD nhóm & thành viên
    void createGroup(const std::string& group);
    void joinGroup(const std::string& group);

    // owner duyệt hoặc từ chối join request
    void approveJoin(const std::string& group, const std::string& user);
    void rejectJoin(const std::string& group, const std::string& user);

    // owner mời thành viên
    void inviteUser(const std::string& group, const std::string& user);

    // thành viên chấp nhận hoặc từ chối lời mời
    void acceptInvite(const std::string& group);
    void rejectInvite(const std::string& group, const std::string& inviter);

    // thành viên rời nhóm
    void leaveGroup(const std::string& group);

    // owner kick thành viên
    void kickMember(const std::string& group, const std::string& user);

    // xem danh sách thành viên
    void listMembers(const std::string& group);

    // lấy cả owned + joined trong 1 lần
    GroupListCache getGroupLists();

    // hiển thị pending queue (join + invite)
    void showPending();
    PendingListResponse getPending();
private:
    Client& client;
};
