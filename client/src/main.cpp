#include "client/Client.h"
#include "client/AuthClient.h"
#include "client/GroupClient.h"
#include "client/FileClient.h"
#include <iostream>

int main() {
    Client client("127.0.0.1", 9000);
    if (!client.connectToServer()) {
        std::cout << "Cannot connect\n";
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
                "\n3. List files"
                "\n0. Logout\n> ";
            int c; std::cin >> c;
            if (c == 1) group.createGroup();
            else if (c == 2) group.joinGroup();
            else if (c == 3) file.listFiles();
            else if (c == 0) loggedIn = false;
        }
    }
    return 0;
}
