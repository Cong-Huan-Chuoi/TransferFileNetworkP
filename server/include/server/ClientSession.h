#pragma once
#include <string>
#include "protocol/bytebuffer.h"

/*
ClientSession
    đại diện cho 1 client đang kết nối
    gắn với 1 socket fd
    lưu trang thái đăng nhập và group
    lưu ByteBuffer để parse TCP stream
*/

struct ClientSession{
    int fd;  //socket của client
    bool logged_in;
    std::string username;
    std::string current_group;
    ByteBuffer buffer; // buffer TCP stream

    ClientSession(int fd_)
        : fd(fd_), logged_in(false){}
};
