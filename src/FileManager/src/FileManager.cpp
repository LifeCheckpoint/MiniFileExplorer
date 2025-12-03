#include "FileManager.h"
#include <algorithm>
#include <sstream>

// 构造函数
FileManager::FileManager() {
    // 初始化为模拟的默认路径
    currentPath = "/home/user/documents";
}

// 析构函数
FileManager::~FileManager() {
    // 占位实现，无需清理
}

// 获取当前工作目录
Status FileManager::getCurrentPath(Path& workingPath) const {
    workingPath = currentPath;
    return Status::Success();
}

// 切换工作目录
Status FileManager::changeDirectory(const Path& workingPath) {
    // 占位实现：简单更新路径
    currentPath = workingPath;
    return Status::Success();
}

// 列出当前工作目录下的所有文件
Status FileManager::listFiles(SortMode sortMode, std::vector<FileInfo>& outFiles) const {
    outFiles.clear();
    
    // 创建一些模拟文件数据
    std::vector<FileInfo> mockFiles = {
        {
            "project.txt",
            currentPath / "project.txt",
            FileType::File,
            2048,
            std::filesystem::file_time_type::clock::now()
        },
        {
            "data",
            currentPath / "data",
            FileType::Directory,
            0,
            std::filesystem::file_time_type::clock::now() - std::chrono::hours(24)
        },
        {
            "readme.md",
            currentPath / "readme.md",
            FileType::File,
            1024,
            std::filesystem::file_time_type::clock::now() - std::chrono::hours(2)
        },
        {
            "config.json",
            currentPath / "config.json",
            FileType::File,
            512,
            std::filesystem::file_time_type::clock::now() - std::chrono::hours(48)
        },
        {
            "backup",
            currentPath / "backup",
            FileType::Directory,
            0,
            std::filesystem::file_time_type::clock::now() - std::chrono::hours(72)
        }
    };
    
    // 根据排序模式排序
    switch (sortMode) {
        case SortMode::BySize:
            std::sort(mockFiles.begin(), mockFiles.end(), 
                [](const FileInfo& a, const FileInfo& b) {
                    return a.size > b.size;
                });
            break;
        case SortMode::ByTime:
            std::sort(mockFiles.begin(), mockFiles.end(),
                [](const FileInfo& a, const FileInfo& b) {
                    return a.modifyTime > b.modifyTime;
                });
            break;
        case SortMode::Default:
        default:
            // 保持默认顺序
            break;
    }
    
    outFiles = mockFiles;
    return Status::Success();
}

// 获取文件详细信息
Status FileManager::getFileStat(const std::string& targetName, FileInfo& outInfo) const {
    // 模拟查找文件
    if (targetName == "project.txt") {
        outInfo = {
            "project.txt",
            currentPath / "project.txt",
            FileType::File,
            2048,
            std::filesystem::file_time_type::clock::now()
        };
        return Status::Success();
    } else if (targetName == "data") {
        outInfo = {
            "data",
            currentPath / "data",
            FileType::Directory,
            4096,
            std::filesystem::file_time_type::clock::now() - std::chrono::hours(24)
        };
        return Status::Success();
    }
    
    return Status::Error(StatusCode::PathNotFound, "文件不存在: " + targetName);
}

// 计算文件夹大小
Status FileManager::calculateDirSize(const Path& dirPath, uintmax_t& outSize) const {
    // 模拟返回固定大小
    outSize = 1024 * 1024; // 1MB
    return Status::Success();
}

// 创建文件（在当前目录）
Status FileManager::createFile(const std::string& filename) {
    // 占位实现：模拟成功
    return Status::Success();
}

// 创建文件（指定路径）
Status FileManager::createFile(const Path& filePath) {
    // 占位实现：模拟成功
    return Status::Success();
}

// 创建文件夹（在当前目录）
Status FileManager::createDirectory(const std::string& dirname) {
    // 占位实现：模拟成功
    return Status::Success();
}

// 创建文件夹（指定路径）
Status FileManager::createDirectory(const Path& dirPath) {
    // 占位实现：模拟成功
    return Status::Success();
}

// 删除文件或文件夹（在当前目录）
Status FileManager::removePath(const std::string& targetName) {
    // 占位实现：模拟成功
    return Status::Success();
}

// 删除文件或文件夹（指定路径）
Status FileManager::removePath(const Path& targetPath) {
    // 占位实现：模拟成功
    return Status::Success();
}

// 复制
Status FileManager::copyItem(const Path& src, const Path& dst) {
    // 占位实现：模拟成功
    return Status::Success();
}

// 移动/重命名
Status FileManager::moveItem(const Path& src, const Path& dst) {
    // 占位实现：模拟成功
    return Status::Success();
}

// 搜索（在当前目录）
Status FileManager::search(const std::string& keyword, std::vector<FileInfo>& outResults) const {
    return search(currentPath, keyword, outResults);
}

// 搜索（指定目录）
Status FileManager::search(const Path& dirPath, const std::string& keyword, std::vector<FileInfo>& outResults) const {
    outResults.clear();
    
    // 模拟搜索结果
    std::vector<FileInfo> allMockFiles = {
        {
            "project.txt",
            dirPath / "project.txt",
            FileType::File,
            2048,
            std::filesystem::file_time_type::clock::now()
        },
        {
            "data",
            dirPath / "data",
            FileType::Directory,
            0,
            std::filesystem::file_time_type::clock::now()
        },
        {
            "readme.md",
            dirPath / "readme.md",
            FileType::File,
            1024,
            std::filesystem::file_time_type::clock::now()
        },
        {
            "project_backup.txt",
            dirPath / "backup" / "project_backup.txt",
            FileType::File,
            2048,
            std::filesystem::file_time_type::clock::now()
        }
    };
    
    // 简单的关键词匹配
    for (const auto& file : allMockFiles) {
        if (file.name.find(keyword) != std::string::npos) {
            outResults.push_back(file);
        }
    }
    
    return Status::Success();
}