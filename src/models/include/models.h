#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <chrono>
#include <filesystem>

// 文件类型枚举
enum class FileType {
    File,
    Directory,
    Unknown
};

// 单个文件或文件夹的详细信息
struct FileInfo {
    std::string name;                           // 文件名
    std::string path;                           // 绝对路径
    FileType type;                              // 类型
    uintmax_t size;                             // 大小 (字节)，文件夹为 0
    std::filesystem::file_time_type modifyTime; // 修改时间
};