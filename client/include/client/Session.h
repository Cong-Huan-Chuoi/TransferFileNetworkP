#pragma once
#include <string>

// ===== CLIENT SESSION =====
struct Session {
    bool logged_in = false;
    std::string username;
    std::string current_group;
};
