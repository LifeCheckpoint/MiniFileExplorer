#pragma once
#include <status.h>

struct Status
{
    StatusCode code;     // 状态码
    std::string message; // 错误信息

    Status(StatusCode c = StatusCode::Success, std::string msg = "") : code(c), message(msg) {}

    bool ok() const { return code == StatusCode::Success; }

    static Status Success() { return Status(StatusCode::Success); }
    static Status Error(StatusCode c, const std::string &msg) { return Status(c, msg); }
};
