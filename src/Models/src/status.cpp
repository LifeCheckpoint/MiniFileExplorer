#include <status.h>

Status::Status(StatusCode c, std::string msg)
    : code(c), message(std::move(msg))
{}

bool Status::ok() const
{
    return code == StatusCode::Success;
}

Status Status::Success()
{
    return Status(StatusCode::Success);
}

Status Status::Error(StatusCode c, const std::string &msg)
{
    return Status(c, msg);
}