#include "FileManager.h"
#include <algorithm>
#include <sstream>

// 构造函数
FileManager::FileManager(const std::string& initPath) {
    // 初始化为模拟的默认路径
    if (initPath.empty()) {
        // 默认加载当前工作目录（getcwd）
        char buf[PATH_MAX];
        if (getcwd(buf, sizeof(buf)) != nullptr) {
            currentPath = fs::path(buf);
        } else {
            currentPath = "/home/user/documents"
            throw std::runtime_error("Failed to get current working directory,set at address /home/user/documents");
        }
    } else {
        // 命令行参数指定初始目录
        fs::path targetPath(initPath);
        if (fs::exists(targetPath) && fs::is_directory(targetPath)) {
            currentPath = targetPath;
        } else {
            throw std::runtime_error("Directory not found: " + initPath);
        }
    }
}
// 析构函数
FileManager::~FileManager() {//释放声明的内存
    // 占位实现，无需清理
    // relieve the memory which newed out of stack or function
}

// 获取当前工作目录
Status FileManager::getCurrentPath(Path& workingPath) const {
    workingPath = currentPath;
    return Status::Success();
}

// 切换工作目录
Status FileManager::changeDirectory(const Path& targetPath) {
    fs::path newPath;

    // 处理 cd ~（切换到用户主目录）
    if (targetPath.string() == "~") {
        const char* homeDir = getenv("HOME");
        if (!homeDir) {
            struct passwd* pwd = getpwuid(getuid());
            if (!pwd) {
                return Status::Error(StatusCode::PathNotFound, "Failed to get home directory");
            }
            homeDir = pwd->pw_dir;
        }
        newPath = fs::path(homeDir);
    } else {
        // 处理相对路径/绝对路径
        newPath = targetPath.is_absolute() ? targetPath : currentPath / targetPath;
        newPath = newPath.lexically_normal(); // 规范化路径（消除 ./ 和 ../）
    }

    // 校验目录合法性
    if (!fs::exists(newPath)) {
        return Status::Error(StatusCode::PathNotFound, "Invalid directory: " + newPath.string());
    }
    if (!fs::is_directory(newPath)) {
        return Status::Error(StatusCode::NotADirectory, "Not a directory: " + newPath.string());
    }

    // 切换成功
    currentPath = newPath;
    return Status::Success();
}

// 辅助函数：文件时间转换为字符串（格式：YYYY-MM-DD HH:MM:SS）
std::string FileManager::fileTimeToString(const fs::file_time_type& fileTime) const {
    auto sysTime = fs::file_clock::to_sys(fileTime);
    auto timeT = system_clock::to_time_t(sysTime);
    std::tm tm = *std::localtime(&timeT);
    std::stringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// 辅助函数：计算目录总大小（递归包含子文件）
uintmax_t FileManager::calculateDirTotalSize(const Path& dirPath) const {
    uintmax_t totalSize = 0;
    for (const auto& entry : fs::recursive_directory_iterator(dirPath)) {
        if (entry.is_regular_file()) {
            totalSize += entry.file_size();
        }
    }
    return totalSize;
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
    //if suucess
    return Status::Success();
    //if error
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