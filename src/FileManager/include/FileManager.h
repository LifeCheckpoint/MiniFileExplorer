// FileManager.h
#pragma once
#include "status.h"
#include "models.h"
#include <filesystem>
#include <vector>

class FileManager {
private:
    std::filesystem::path currentPath;

public:
    FileManager();

    // --- 1. 状态查询与导航 ---

    // 获取当前路径字符串
    // [Out] outPath: 传出当前路径
    Status getCurrentPath(std::string& outPath) const;

    // 切换目录
    // [In] path: 目标路径
    Status changeDirectory(const std::string& path);

    // --- 2. 列表与信息查询 ---

    // 列出当前目录下的所有文件
    // [In] sortMode: 0=默认, 1=大小, 2=时间
    // [Out] outFiles: 传出文件列表
    Status listFiles(int sortMode, std::vector<FileInfo>& outFiles) const;

    // 获取指定目标的详细信息
    // [In] targetName: 目标名称
    // [Out] outInfo: 传出详细信息
    Status getFileStat(const std::string& targetName, FileInfo& outInfo) const;

    // 计算文件夹大小
    // [In] dirName: 文件夹名称
    // [Out] outSize: 传出总字节数
    Status calculateDirSize(const std::string& dirName, uintmax_t& outSize) const;

    // --- 3. 文件操作 ---

    // 创建文件
    Status createFile(const std::string& filename);

    // 创建文件夹
    Status createDirectory(const std::string& dirname);

    // 删除文件或文件夹
    Status removePath(const std::string& targetName);

    // 复制
    Status copyItem(const std::string& src, const std::string& dst);

    // 移动/重命名
    Status moveItem(const std::string& src, const std::string& dst);

    // --- 4. 进阶功能 ---

    // 搜索
    // [In] keyword: 关键词
    // [Out] outResults: 传出匹配的文件列表
    Status search(const std::string& keyword, std::vector<FileInfo>& outResults) const;
};
