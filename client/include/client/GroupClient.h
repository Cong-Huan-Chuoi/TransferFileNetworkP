#pragma once

#include "Client.h"
#include <vector>
#include <string>

struct GroupListCache {
    std::vector<std::string> owned;
    std::vector<std::string> joined;
};

class GroupClient {
public:
    explicit GroupClient(Client& c);

    void createGroup();
    void joinGroup();
    void approveJoin();     // owner duyệt yêu cầu
    void inviteUser();      // owner mời thành viên
    void acceptInvite();    // thành viên chấp nhận lời mời
    void leaveGroup();      // thành viên rời nhóm
    void kickMember();      // owner kick thành viên
    void listMembers();     // xem danh sách thành viên

    GroupListCache getGroupLists(); // lấy cả owned + joined trong 1 lần

private:
    Client& client;
};
