#pragma once
#include <string>
#include <filesystem>

class DataPaths {
public:
    // ===== EXISTING CONTRACT =====
    static std::string groupsDir();
    static std::string usersDb();
    static std::string groupsDb();

    // ===== CORE STABILIZATION ADD =====
    static void init(const std::filesystem::path& executablePath);

    static std::filesystem::path dataRoot();
    static std::filesystem::path groupRoot(const std::string& groupId);
    static std::filesystem::path groupFiles(const std::string& groupId);

private:
    static std::filesystem::path projectRoot;
    static std::filesystem::path ensureDir(const std::filesystem::path& p);
};
