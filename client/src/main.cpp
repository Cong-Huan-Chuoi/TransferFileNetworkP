#include "client/Client.h"
#include "client/AuthClient.h"
#include "client/GroupClient.h"
#include "client/FileClient.h"
#include "protocol/packets.h"
#include <iostream>

int main() {
    Client client("127.0.0.1", 9000);
    if (!client.connectToServer()) {
        std::cout << "Cannot connect to server.\n";
        return 1;
    }

    AuthClient auth(client);
    GroupClient group(client);
    FileClient file(client);

    bool loggedIn = false;
    std::string currentGroup;

    while (true) {
        // 1.Client_Entry
        if (!loggedIn) {
            std::cout << "\n=== 1.Client_Entry ===\n"
                      << "1.1 Register\n"
                      << "1.2 Login\n"
                      << "1.3 Exit\n> ";
            int c; std::cin >> c;
            if (c == 1) auth.registerUser();
            else if (c == 2) auth.loginUser(loggedIn);
            else if (c == 3) break;
        } else {
            // 2.User_Dashboard
            std::cout << "\n=== 2.User_Dashboard ===\n"
                      << "2.1 Create group\n"
                      << "2.2 Join group\n"
                      << "2.3 List joined groups\n"
                      << "2.4 List owned groups\n"
                      << "2.5 Pending invited queue\n"
                      << "2.6 Log out\n> ";
            int c; std::cin >> c;

            if (c == 1) {
                std::string g; std::cout << "Group name: "; std::cin >> g;
                group.createGroup(g);
            } else if (c == 2) {
                std::string g; std::cout << "Group name: "; std::cin >> g;
                group.joinGroup(g);
            } else if (c == 3) {
                auto lists = group.getGroupLists();
                if (lists.joined.empty()) { std::cout << "No joined groups.\n"; continue; }
                for (size_t i=0;i<lists.joined.size();++i)
                    std::cout << (i+1) << ". " << lists.joined[i] << "\n";
                int choice; std::cout << "Select group: "; std::cin >> choice;
                if (choice<1 || choice>(int)lists.joined.size()) continue;
                currentGroup = lists.joined[choice-1];

                // Access joined group
                while (true) {
                    std::cout << "\n[Access Joined Group: " << currentGroup << "]\n"
                              << "2.3.1 Group operation\n"
                              << "2.3.2 Folder operation\n"
                              << "0. Back to dashboard\n> ";
                    int sub; std::cin >> sub;
                    if (sub==0) break;
                    else if (sub==1) {
                        while (true) {
                            std::cout << "\n-- Group operation --\n"
                                      << "1.list_user\n2.invite_user\n3.leave_group\n0.Back\n> ";
                            int op; std::cin >> op;
                            if (op==0) break;
                            else if (op==1) group.listMembers(currentGroup);
                            else if (op==2) { std::string u; std::cout<<"User: "; std::cin>>u; group.inviteUser(currentGroup,u); }
                            else if (op==3) group.leaveGroup(currentGroup);
                        }
                    } else if (sub==2) {
                        while (true) {
                            std::cout << "\n-- Folder operation --\n"
                                      << "1.list_folder\n2.create_folder\n3.upload_file\n4.download_file\n0.Back\n> ";
                            int fo; std::cin >> fo;
                            if (fo==0) break;
                            else if (fo==1) file.listFolder();
                            else if (fo==2) file.createFolder();
                            else if (fo==3) file.uploadFile();
                            else if (fo==4) file.downloadFile();
                        }
                    }
                }
            } else if (c == 4) {
                auto lists = group.getGroupLists();
                if (lists.owned.empty()) { std::cout << "No owned groups.\n"; continue; }
                for (size_t i=0;i<lists.owned.size();++i)
                    std::cout << (i+1) << ". " << lists.owned[i] << "\n";
                int choice; std::cout << "Select group: "; std::cin >> choice;
                if (choice<1 || choice>(int)lists.owned.size()) continue;
                currentGroup = lists.owned[choice-1];

                // Access owned group
                while (true) {
                    std::cout << "\n[Access Owned Group: " << currentGroup << "]\n"
                              << "2.4.1 Group management\n"
                              << "2.4.2 Advanced folder operation\n"
                              << "0. Back to dashboard\n> ";
                    int sub; std::cin >> sub;
                    if (sub==0) break;
                    else if (sub==1) {
                        while (true) {
                            std::cout << "\n-- Group management --\n"
                                      << "1.list_user\n2.invite_user\n3.kick_user\n4.pending_queue_management\n0.Back\n> ";
                            int gm; std::cin >> gm;
                            if (gm==0) break;
                            else if (gm==1) group.listMembers(currentGroup);
                            else if (gm==2) { std::string u; std::cout<<"User: "; std::cin>>u; group.inviteUser(currentGroup,u); }
                            else if (gm==3) { std::string u; std::cout<<"User: "; std::cin>>u; group.kickMember(currentGroup,u); }
                            else if (gm==4) {
                                // Owner pending queue management
                                PendingListResponse res;
                                group.showPending(); // hiển thị danh sách
                                std::cout << "Select join request number (0.Back): ";
                                int idx; std::cin >> idx;
                                if (idx>0 && idx<=res.joinRequests.size()) {
                                    auto entry = res.joinRequests[idx-1];
                                    std::cout << "1.Approve 2.Reject > ";
                                    int act; std::cin >> act;
                                    if (act==1) group.approveJoin(entry.groupName, entry.username);
                                    else if (act==2) group.rejectJoin(entry.groupName, entry.username);
                                }
                            }
                        }
                    } else if (sub==2) {
                        while (true) {
                            std::cout << "\n-- Advanced folder operation --\n"
                                      << "1.list_folder\n2.create_folder\n3.upload_file\n4.download_file\n5.remove\n6.rename\n7.copy\n8.move\n0.Back\n> ";
                            int fo; std::cin >> fo;
                            if (fo==0) break;
                            else if (fo==1) file.listFolder();
                            else if (fo==2) file.createFolder();
                            else if (fo==3) file.uploadFile();
                            else if (fo==4) file.downloadFile();
                            else if (fo==5) file.deleteEntry();
                            else if (fo==6) file.renameEntry();
                            else if (fo==7) file.copyEntry();
                            else if (fo==8) file.moveEntry();
                        }
                    }
                }
            } else if (c == 5) {
                // Pending invited queue (member side)
                PendingListResponse res;
                group.showPending(); // hiển thị danh sách
                std::cout << "Select invite number (0.Back): ";
                int idx; std::cin >> idx;
                if (idx>0 && idx<=res.invites.size()) {
                    auto entry = res.invites[idx-1];
                    std::cout << "1.Accept 2.Reject > ";
                    int act; std::cin >> act;
                    if (act==1) group.acceptInvite(entry.groupName);
                    else if (act==2) group.rejectInvite(entry.groupName, entry.username);
                }
            } else if (c == 6) {
                loggedIn = false;
            }
        }
    }
    return 0;
}
