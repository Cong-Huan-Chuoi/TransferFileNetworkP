#include "client/Client.h"
#include "client/AuthClient.h"
#include "client/GroupClient.h"
#include "client/FileClient.h"
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

    while (true) {
        if (!loggedIn) {
            std::cout << "\n1. Register\n2. Login\n0. Exit\n> ";
            int c; std::cin >> c;
            if (c == 1) auth.registerUser();
            else if (c == 2) auth.loginUser(loggedIn);
            else break;
        } else {
            std::cout <<
                "\n1. Create group"
                "\n2. Join group"
                "\n3. List joined groups"
                "\n4. List owned groups"
                "\n5. Pending invited queue (accept invite)"
                "\n6. Log out\n> ";
            int c; std::cin >> c;

            if (c == 1) {
                group.createGroup();
            } else if (c == 2) {
                group.joinGroup();
            } else if (c == 3 || c == 4) {
                std::cout << "[Client UI] Requesting group lists for user\n";
                auto lists = group.getGroupLists();
                const auto& groups = (c == 3) ? lists.joined : lists.owned;

                if (groups.empty()) {
                    std::cout << (c == 3 ? "You haven't joined any groups.\n"
                                          : "You don't own any groups.\n");
                    continue;
                }

                std::cout << (c == 3 ? "Joined groups:\n" : "Owned groups:\n");
                for (size_t i = 0; i < groups.size(); ++i)
                    std::cout << " " << (i + 1) << ". " << groups[i] << "\n";

                std::cout << "Select group number to access (0 to cancel): ";
                int choice; std::cin >> choice;
                if (choice < 1 || choice > (int)groups.size()) continue;

                std::string g = groups[choice - 1];
                while (true) {
                    std::cout << "\n[Group: " << g << "]\n";
                    if (c == 3) {
                        std::cout <<
                            "1. Group operation (list/invite/leave)\n"
                            "2. Folder operation (list/create/upload/download)\n"
                            "0. Back\n> ";
                    } else {
                        std::cout <<
                            "1. Group management (list/invite/kick/approve)\n"
                            "2. Advanced folder operation\n"
                            "0. Back\n> ";
                    }

                    int sub; std::cin >> sub;
                    if (sub == 0) break;

                    if (sub == 1) {
                        while (true) {
                            if (c == 3) {
                                std::cout <<
                                    "\nGroup operation\n"
                                    "1. List members\n"
                                    "2. Invite user\n"
                                    "3. Leave group\n"
                                    "0. Back\n> ";
                                int op; std::cin >> op;
                                if (op == 0) break;
                                else if (op == 1) group.listMembers();
                                else if (op == 2) group.inviteUser();
                                else if (op == 3) group.leaveGroup();
                            } else {
                                std::cout <<
                                    "\nGroup management\n"
                                    "1. List members\n"
                                    "2. Invite user\n"
                                    "3. Kick user\n"
                                    "4. Approve join request\n"
                                    "0. Back\n> ";
                                int gm; std::cin >> gm;
                                if (gm == 0) break;
                                else if (gm == 1) group.listMembers();
                                else if (gm == 2) group.inviteUser();
                                else if (gm == 3) group.kickMember();
                                else if (gm == 4) group.approveJoin();
                            }
                        }
                    } else if (sub == 2) {
                        while (true) {
                            std::cout <<
                                "\nFolder operation\n"
                                "1. List folder\n"
                                "2. Create folder\n"
                                "3. Upload file\n"
                                "4. Download file\n";
                            if (c == 4) {
                                std::cout <<
                                    "5. Delete file/folder\n"
                                    "6. Rename file/folder\n"
                                    "7. Copy file/folder\n"
                                    "8. Move file/folder\n";
                            }
                            std::cout << "0. Back\n> ";

                            int fo; std::cin >> fo;
                            if (fo == 0) break;
                            else if (fo == 1) file.listFolder();
                            else if (fo == 2) file.createFolder();
                            else if (fo == 3) file.uploadFile();
                            else if (fo == 4) file.downloadFile();
                            else if (c == 4) {
                                if (fo == 5) file.deleteEntry();
                                else if (fo == 6) file.renameEntry();
                                else if (fo == 7) file.copyEntry();
                                else if (fo == 8) file.moveEntry();
                            }
                        }
                    }
                }
            } else if (c == 5) {
                group.acceptInvite();
            } else if (c == 6) {
                loggedIn = false;
            }
        }
    }

    return 0;
}
