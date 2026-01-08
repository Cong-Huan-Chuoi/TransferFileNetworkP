#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <cstdint>

/*
 * FileSystemManager
 *  - Thao tác với filesystem thật
 *  - Root: data/groups/
 */
class FileSystemManager {
public:
    explicit FileSystemManager(const std::string& base_dir);

    /* ===== Directory ===== */
    bool create_group_root(const std::string& group);
    bool create_dir(const std::string& group,
                    const std::string& relative_path);

    bool remove_dir(const std::string& group,
                    const std::string& relative_path);

    bool rename_dir(const std::string& group,
                    const std::string& old_path,
                    const std::string& new_path);

    /* ===== File ===== */
    bool remove_file(const std::string& group,
                     const std::string& relative_path);

    bool rename_file(const std::string& group,
                     const std::string& old_path,
                     const std::string& new_path);

    bool copy_file(const std::string& group,
                   const std::string& src,
                   const std::string& dst);

    bool move_file(const std::string& group,
                   const std::string& src,
                   const std::string& dst);

    /* ===== Listing ===== */
    std::vector<std::string> list_dir(const std::string& group,
                                      const std::string& relative_path) const;

    /* ===== Upload / Download ===== */
    bool write_file_chunk(const std::string& group,
                          const std::string& relative_path,
                          const uint8_t* data,
                          size_t size,
                          bool append);

    bool read_file_chunk(const std::string& group,
                          const std::string& relative_path,
                          size_t offset,
                          size_t size,
                          std::vector<uint8_t>& out) const;

private:
    std::filesystem::path resolve_path(const std::string& group,
                                       const std::string& relative_path) const;

private:
    std::filesystem::path base;
};
