#include "client/Client.h"
#include "client/AuthClient.h"
#include "client/GroupClient.h"
#include "client/FileClient.h"
#include "protocol/packets.h"
#include <iostream>
#include <stdexcept>
#include <limits>
#include <string>

/**
 * PROJECT: File Sharing Application (C++ on Linux)
 * TARGET: HUST - Software Development Management - 2026.1
 */

int main() {
    // Khởi tạo kết nối tới server
    Client client("127.0.0.1", 9000);
    if (!client.connectToServer()) {
        std::cerr << "Error: Cannot connect to server at 127.0.0.1:9000.\n";
        return 1;
    }

    // Các thành phần xử lý nghiệp vụ
    AuthClient auth(client);
    GroupClient group(client);
    FileClient file(client);

    bool loggedIn = false;
    std::string currentGroup;

    // Vòng lặp chính của chương trình
    while (true) {
        try {
            // 1. GIAI ĐOẠN CHƯA ĐĂNG NHẬP (Client_Entry)
            if (!loggedIn) {
                std::cout << "\n=== 1.Client_Entry ===\n"
                          << "1.1 Register\n"
                          << "1.2 Login\n"
                          << "1.3 Exit\n> ";
                int c;
                if (!(std::cin >> c)) {
                    throw std::invalid_argument("Input must be a number.");
                }

                if (c == 1) {
                    bool ok = auth.registerUser();
                    std::cout << (ok ? "Registration successful.\n" : "Registration failed.\n");
                    continue;
                } 
                else if (c == 2) {
                    auth.loginUser(loggedIn);
                } 
                else if (c == 3) {
                    break;
                } 
                else {
                    std::cout << "Invalid choice. Please select 1, 2, or 3.\n";
                }
            } 
            // 2. GIAI ĐOẠN ĐÃ ĐĂNG NHẬP (User_Dashboard)
            else {
                std::cout << "\n=== 2.User_Dashboard ===\n"
                          << "2.1 Create group\n"
                          << "2.2 Join group\n"
                          << "2.3 List joined groups\n"
                          << "2.4 List owned groups\n"
                          << "2.5 Pending invited queue\n"
                          << "2.6 Log out\n> ";
                int c;
                if (!(std::cin >> c)) {
                    throw std::invalid_argument("Input must be a number.");
                }

                if (c == 1) {
                    std::string g; std::cout << "Group name to create: "; std::cin >> g;
                    group.createGroup(g);
                } 
                else if (c == 2) {
                    std::string g; std::cout << "Group name to join: "; std::cin >> g;
                    group.joinGroup(g);
                } 
                else if (c == 3) {
                    auto lists = group.getGroupLists();
                    if (lists.joined.empty()) {
                        std::cout << "No joined groups.\n";
                        continue;
                    }
                    for (size_t i = 0; i < lists.joined.size(); ++i)
                        std::cout << (i + 1) << ". " << lists.joined[i] << "\n";
                    
                    int choice; std::cout << "Select group (or 0 to back): "; std::cin >> choice;
                    if (std::cin.fail()) throw std::invalid_argument("Expected a number.");
                    if (choice == 0) continue;
                    if (choice < 1 || choice > (int)lists.joined.size()) throw std::out_of_range("Invalid group index.");
                    
                    currentGroup = lists.joined[choice - 1];
                    client.getSession().current_group = currentGroup;

                    // Menu thao tác trong nhóm đã tham gia (Joined Group)
                    while (true) {
                        std::cout << "\n[Access Joined Group: " << currentGroup << "]\n"
                                  << "2.3.1 Group operation\n"
                                  << "2.3.2 Folder operation\n"
                                  << "0. Back to dashboard\n> ";
                        int sub; std::cin >> sub;
                        if (std::cin.fail()) throw std::invalid_argument("Expected a number.");
                        if (sub == 0) break;
                        
                        else if (sub == 1) { // Group operation
                            while (true) {
                                std::cout << "\n-- Group operation --\n"
                                          << "1.list_user\n2.invite_user\n3.leave_group\n0.Back\n> ";
                                int op; std::cin >> op;
                                if (std::cin.fail()) throw std::invalid_argument("Expected a number.");
                                if (op == 0) break;
                                else if (op == 1) group.listMembers(currentGroup);
                                else if (op == 2) { std::string u; std::cout << "User: "; std::cin >> u; group.inviteUser(currentGroup, u); }
                                else if (op == 3) { group.leaveGroup(currentGroup); break; }
                            }
                        } 
                        else if (sub == 2) { // Folder operation
                            while (true) {
                                std::cout << "\n-- Folder operation --\n"
                                          << "1.list_folder\n2.create_folder\n3.upload_file\n4.download_file\n0.Back\n> ";
                                int fo; std::cin >> fo;
                                if (std::cin.fail()) throw std::invalid_argument("Expected a number.");
                                if (fo == 0) break;
                                else if (fo == 1) file.listFolder();
                                else if (fo == 2) file.createFolder();
                                else if (fo == 3) file.uploadFile();
                                else if (fo == 4) file.downloadFile();
                            }
                        }
                    }
                } 
                else if (c == 4) {
                    auto lists = group.getGroupLists();
                    if (lists.owned.empty()) {
                        std::cout << "No owned groups.\n";
                        continue;
                    }
                    for (size_t i = 0; i < lists.owned.size(); ++i)
                        std::cout << (i + 1) << ". " << lists.owned[i] << "\n";
                    
                    int choice; std::cout << "Select group to manage (or 0 to back): "; std::cin >> choice;
                    if (std::cin.fail()) throw std::invalid_argument("Expected a number.");
                    if (choice == 0) continue;
                    if (choice < 1 || choice > (int)lists.owned.size()) throw std::out_of_range("Invalid group index.");
                    
                    currentGroup = lists.owned[choice - 1];
                    client.getSession().current_group = currentGroup;
                    // Menu quản lý nhóm sở hữu (Owned Group)
                    while (true) {
                        std::cout << "\n[Access Owned Group: " << currentGroup << "]\n"
                                  << "2.4.1 Group management\n"
                                  << "2.4.2 Advanced folder operation\n"
                                  << "0. Back to dashboard\n> ";
                        int sub; std::cin >> sub;
                        if (std::cin.fail()) throw std::invalid_argument("Expected a number.");
                        if (sub == 0) break;
                        
                        else if (sub == 1) { // Group management
                            while (true) {
                                std::cout << "\n-- Group management --\n"
                                          << "1.list_user\n2.invite_user\n3.kick_user\n4.pending_queue_management\n0.Back\n> ";
                                int gm; std::cin >> gm;
                                if (std::cin.fail()) throw std::invalid_argument("Expected a number.");
                                if (gm == 0) break;
                                else if (gm == 1) group.listMembers(currentGroup);
                                else if (gm == 2) { std::string u; std::cout << "User: "; std::cin >> u; group.inviteUser(currentGroup, u); }
                                else if (gm == 3) { std::string u; std::cout << "User: "; std::cin >> u; group.kickMember(currentGroup, u); }
                                else if (gm == 4) {
                                    auto res = group.getPending();
                                    std::cout << "Pending join requests:\n";
                                    for (size_t i = 0; i < res.joinRequests.size(); ++i) {
                                        std::cout << (i + 1) << ". " << res.joinRequests[i].username
                                                  << " want to join " << res.joinRequests[i].groupName << "\n";
                                    }
                                    std::cout << "Select request (0.Back): ";
                                    int idx; std::cin >> idx;
                                    if (std::cin.fail()) throw std::invalid_argument("Expected a number.");
                                    if (idx > 0 && idx <= (int)res.joinRequests.size()) {
                                        auto entry = res.joinRequests[idx - 1];
                                        std::cout << "1.Approve 2.Reject > ";
                                        int act; std::cin >> act;
                                        if (act == 1) group.approveJoin(entry.groupName, entry.username);
                                        else if (act == 2) group.rejectJoin(entry.groupName, entry.username);
                                    }
                                }
                            }
                        } 
                        else if (sub == 2) { // Advanced folder operation
                            while (true) {
                                std::cout << "\n-- Advanced folder operation --\n"
                                          << "1.list\n2.create\n3.upload\n4.download\n5.remove\n6.rename\n7.copy\n8.move\n0.Back\n> ";
                                int fo; std::cin >> fo;
                                if (std::cin.fail()) throw std::invalid_argument("Expected a number.");
                                if (fo == 0) break;
                                else if (fo == 1) file.listFolder();
                                else if (fo == 2) file.createFolder();
                                else if (fo == 3) file.uploadFile();
                                else if (fo == 4) file.downloadFile();
                                else if (fo == 5) file.deleteEntry();
                                else if (fo == 6) file.renameEntry();
                                else if (fo == 7) file.copyEntry();
                                else if (fo == 8) file.moveEntry();
                            }
                        }
                    }
                } 
                else if (c == 5) { // Pending invited queue
                    auto res = group.getPending();
                    if (res.invites.empty()) {
                        std::cout << "No pending invites.\n";
                    } else {
                        for (size_t i = 0; i < res.invites.size(); ++i)
                            std::cout << (i + 1) << ". Invited to " << res.invites[i].groupName << " by " << res.invites[i].inviter << "\n";

                        std::cout << "Select invite (0.Back): ";
                        int idx; std::cin >> idx;
                        if (std::cin.fail()) throw std::invalid_argument("Expected a number.");
                        if (idx > 0 && idx <= (int)res.invites.size()) {
                            auto entry = res.invites[idx - 1];
                            std::cout << "1.Accept 2.Reject > ";
                            int act; std::cin >> act;
                            if (act == 1) group.acceptInvite(entry.groupName);
                            else if (act == 2) group.rejectInvite(entry.groupName, entry.inviter);
                        }
                    }
                } 
                else if (c == 6) {
                    loggedIn = false;
                    std::cout << "Logged out successfully.\n";
                }
            }
        }
        catch (const std::invalid_argument& e) {
            std::cin.clear(); 
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "\n[Input Error]: " << e.what() << "\n";
        }
        catch (const std::out_of_range& e) {
            std::cout << "\n[Selection Error]: " << e.what() << "\n";
        }
        catch (const std::exception& e) {
            std::cout << "\n[Unexpected Error]: " << e.what() << "\n";
        }
    }

    std::cout << "Exiting application...\n";
    return 0;
}