#include "client/FileClient.h"
#include "protocol/packet_types.h"
#include <iostream>

FileClient::FileClient(Client& c) : client(c) {}

void FileClient::listFiles() {
    std::cout << "List files (demo stub)\n";
}
