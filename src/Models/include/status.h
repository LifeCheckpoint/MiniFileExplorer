#pragma once//预处理指令，用于防止头文件被重复包含
#include <string>

enum class StatusCode//show the error information
{
    Success = 0,

    // 通用错误
    UnknownError,
    InvalidArguments,

    // 文件系统错误
    PathNotFound,      // 路径不存在
    PathAlreadyExists, // 路径已存在
    NotADirectory,     // 不是文件夹
    NotAFile,          // 不是文件
    PermissionDenied,  // 权限不足
    NotEmpty,          // 文件夹非空 (rmdir时)

    CopyFailed,//拷贝失败
    MoveFailed//移动失败
};

struct Status
{
    StatusCode code;     // 状态码
    std::string message; // 错误信息

    Status(StatusCode c = StatusCode::Success, std::string msg = "");

    bool ok() const;

    static Status Success();
    static Status Error(StatusCode c, const std::string &msg);
};
