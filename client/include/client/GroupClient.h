#pragma once
#include "Client.h"

class GroupClient {
public:
    explicit GroupClient(Client& c);

    void createGroup();
    void joinGroup();
    void approveJoin();     // trưởng nhóm duyệt yêu cầu
    void inviteUser();      // trưởng nhóm mời thành viên
    void acceptInvite();    // thành viên chấp nhận lời mời
    void leaveGroup();      // thành viên rời nhóm
    void kickMember();      // trưởng nhóm kick thành viên
    void listMembers();     // xem danh sách thành viên
    void listGroups();

private:
    Client& client;
};
