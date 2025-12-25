#pragma once

#include "status.h"
#include "models.h"
#include <filesystem>
#include <vector>

using Path = std::filesystem::path;

class FileManager {

private:
    std::filesystem::path currentPath;

    // 辅助函数
    uintmax_t calculateDirTotalSize(const Path& dirPath) const;
    bool pathExists(const Path& targetPath) const;
    std::string fileTimeToString(const std::filesystem::file_time_type& fileTime) const;

public:
    // 构造函数
    FileManager(const std::string& initPath = "");
    // 析构函数
    ~FileManager();


    // 获取工作目录
    // [Out] workingPath: 当前工作目录
    Status getCurrentPath(Path& workingPath) const;


    // 切换工作目录
    // [In] workingPath: 目标工作目录
    Status changeDirectory(const Path& workingPath);


    // 列出当前工作目录下的所有文件
    // [In]  sortMode: 排序方式
    // [Out] outFiles: 传出文件列表
    Status listFiles(SortMode sortMode, std::vector<FileInfo>& outFiles) const;


    // 获取当前工作目录下指定名称文件 / 文件夹的详细信息
    // [In]  targetName: 目标名称
    // [Out] outInfo: 传出详细信息
    Status getFileStat(const std::string& targetName, FileInfo& outInfo) const;


    // 计算文件夹大小
    // [In]  dirPath: 文件夹路径
    // [Out] outSize: 传出总字节数
    Status calculateDirSize(const Path& dirPath, uintmax_t& outSize) const;


    // 创建文件
    // 在当前工作目录创建文件
    // [In] filename: 文件名
    Status createFile(const std::string& filename);
    // 在指定路径创建文件
    // [In] filePath: 文件路径
    Status createFile(const Path& filePath);


    // 创建文件夹
    // 在当前工作目录创建文件夹
    // [In] dirname: 文件夹名
    Status createDirectory(const std::string& dirname);
    // 在指定路径创建文件夹
    // [In] dirPath: 文件夹路径
    Status createDirectory(const Path& dirPath);


    // 删除文件或文件夹
    // 在当前工作目录删除指定名称的文件或文件夹
    // [In] targetName: 目标名称
    Status removePath(const std::string& targetName);
    // 删除指定路径文件或文件夹
    // [In] targetPath: 目标路径
    Status removePath(const Path& targetPath);


    // 复制
    // [In] src: 源路径
    // [In] dst: 目标路径
    Status copyItem(const Path& src, const Path& dst);


    // 移动 / 重命名
    // [In] src: 源路径
    // [In] dst: 目标路径
    Status moveItem(const Path& src, const Path& dst);


    // 搜索
    // 在当前工作目录及其子目录中搜索
    // [In]  keyword: 文件名关键词
    // [Out] outResults: 传出匹配的文件列表
    Status search(const std::string& keyword, std::vector<FileInfo>& outResults) const;
    // 在指定目录及其子目录中搜索
    // [In]  dirPath: 目标目录
    // [In]  keyword: 文件名关键词
    // [Out] outResults: 传出匹配的文件列表
    Status search(const Path& dirPath, const std::string& keyword, std::vector<FileInfo>& outResults) const;
};
