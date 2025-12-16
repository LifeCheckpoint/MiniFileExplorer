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

// 列出当前目录文件（支持按大小/时间排序）
Status FileManager::listFiles(SortMode sortMode, std::vector<FileInfo>& outFiles) const {
    outFiles.clear();

    // 遍历当前目录
    try {
        for (const auto& entry : fs::directory_iterator(currentPath)) {
            FileInfo info;
            info.name = entry.path().filename().string();
            info.path = entry.path();
            info.type = entry.is_directory() ? FileType::Directory : FileType::File;
            info.modifyTime = entry.last_write_time();

            // 设置大小（文件：字节数；目录：-，排序时用总大小）
            if (info.type == FileType::File) {
                info.size = entry.file_size();
            } else {
                info.size = 0; // 列表显示为 "-"，排序时单独处理
                info.dirTotalSize = calculateDirTotalSize(entry.path());
            }

            outFiles.push_back(info);
        }
    } catch (const fs::filesystem_error& e) {
        return Status::Error(StatusCode::PermissionDenied, "Permission denied: " + std::string(e.what()));
    }

    // 根据排序模式排序
    switch (sortMode) {
        case SortMode::BySize:
            // 按大小降序：文件用自身大小，目录用总大小，空目录排最后
            std::sort(outFiles.begin(), outFiles.end(),
                [](const FileInfo& a, const FileInfo& b) {
                    uintmax_t sizeA = (a.type == FileType::File) ? a.size : a.dirTotalSize;
                    uintmax_t sizeB = (b.type == FileType::File) ? b.size : b.dirTotalSize;
                    return sizeA > sizeB;
                });
            break;
        case SortMode::ByTime:
            // 按修改时间降序（最新在前）
            std::sort(outFiles.begin(), outFiles.end(),
                [](const FileInfo& a, const FileInfo& b) {
                    return a.modifyTime > b.modifyTime;
                });
            break;
        case SortMode::Default:
        default:
            // 默认按名称字典序排序
            std::sort(outFiles.begin(), outFiles.end(),
                [](const FileInfo& a, const FileInfo& b) {
                    return a.name < b.name;
                });
            break;
    }

    return Status::Success();
}

Status FileManager::getFileStat(const std::string& targetName, FileInfo& outInfo) const {
    if (targetName.empty()) {
        return Status::Error(StatusCode::InvalidArguments, "Missing target: Please enter 'stat [name]'");
    }

    fs::path targetPath = currentPath / targetName;
    if (!fs::exists(targetPath)) {
        return Status::Error(StatusCode::PathNotFound, "Target not found: " + targetName);
    }

    // 填充文件信息
    outInfo.name = targetPath.filename().string();
    outInfo.path = targetPath;
    outInfo.type = fs::is_directory(targetPath) ? FileType::Directory : FileType::File;
    outInfo.modifyTime = fs::last_write_time(targetPath);

    // 大小：文件为字节数，目录为 "-"
    if (outInfo.type == FileType::File) {
        outInfo.size = fs::file_size(targetPath);
    } else {
        outInfo.size = 0; // 显示时替换为 "-"
    }

    // 补充创建时间和访问时间（Linux 下需额外处理，这里简化）
    outInfo.createTime = fs::last_write_time(targetPath); // 实际应获取创建时间
    outInfo.accessTime = fs::last_write_time(targetPath); // 实际应获取访问时间

    return Status::Success();
}

// 计算文件夹总大小（du 命令，自动适配 KB/MB）
Status FileManager::calculateDirSize(const Path& dirPath, uintmax_t& outSize) const {
    fs::path targetPath = dirPath.is_absolute() ? dirPath : currentPath / dirPath;

    // 校验目录合法性
    if (!fs::exists(targetPath)) {
        return Status::Error(StatusCode::PathNotFound, "Directory not found: " + targetPath.string());
    }
    if (!fs::is_directory(targetPath)) {
        return Status::Error(StatusCode::NotADirectory, "Not a directory: " + targetPath.string());
    }

    // 递归计算总大小
    outSize = calculateDirTotalSize(targetPath);
    return Status::Success();
}

// 辅助函数：检查文件/目录是否已存在
bool FileManager::pathExists(const Path& targetPath) const {
    return fs::exists(targetPath);
}

// 创建文件（touch 命令）
Status FileManager::createFile(const std::string& filename) {
    if (filename.empty()) {
        return Status::Error(StatusCode::InvalidArguments, "Missing filename: Please enter 'touch [filename]'");
    }

    fs::path filePath = currentPath / filename;
    if (pathExists(filePath)) {
        return Status::Error(StatusCode::PathAlreadyExists, "File already exists: " + filename);
    }

    // 创建空文件
    std::ofstream file(filePath);
    if (!file.is_open()) {
        return Status::Error(StatusCode::PermissionDenied, "Permission denied: Cannot create file " + filename);
    }
    file.close();

    return Status::Success();
}

// 创建文件（指定路径重载）
Status FileManager::createFile(const Path& filePath) {
    fs::path targetPath = filePath.is_absolute() ? filePath : currentPath / filePath;
    if (pathExists(targetPath)) {
        return Status::Error(StatusCode::PathAlreadyExists, "File already exists: " + targetPath.string());
    }

    // 创建父目录（如果不存在）
    fs::path parentDir = targetPath.parent_path();
    if (!fs::exists(parentDir)) {
        fs::create_directories(parentDir);
    }

    // 创建空文件
    std::ofstream file(targetPath);
    if (!file.is_open()) {
        return Status::Error(StatusCode::PermissionDenied, "Permission denied: Cannot create file " + targetPath.string());
    }
    file.close();

    return Status::Success();
}

// 创建文件夹（mkdir 命令）
Status FileManager::createDirectory(const std::string& dirname) {
    if (dirname.empty()) {
        return Status::Error(StatusCode::InvalidArguments, "Missing directory name: Please enter 'mkdir [dirname]'");
    }

    fs::path dirPath = currentPath / dirname;
    if (pathExists(dirPath)) {
        return Status::Error(StatusCode::PathAlreadyExists, "Directory already exists: " + dirname);
    }

    // 创建文件夹
    try {
        fs::create_directory(dirPath);
    } catch (const fs::filesystem_error& e) {
        return Status::Error(StatusCode::PermissionDenied, "Permission denied: Cannot create directory " + dirname);
    }

    return Status::Success();
}

// 创建文件夹（指定路径重载）
Status FileManager::createDirectory(const Path& dirPath) {
    fs::path targetPath = dirPath.is_absolute() ? dirPath : currentPath / dirPath;
    if (pathExists(targetPath)) {
        return Status::Error(StatusCode::PathAlreadyExists, "Directory already exists: " + targetPath.string());
    }

    // 创建文件夹（递归创建父目录）
    try {
        fs::create_directories(targetPath);
    } catch (const fs::filesystem_error& e) {
        return Status::Error(StatusCode::PermissionDenied, "Permission denied: Cannot create directory " + targetPath.string());
    }

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