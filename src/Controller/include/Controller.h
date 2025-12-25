#pragma once
#include <functional>
#include "FileManager.h"
#include "CommandParser.h"

class Controller {
public:
    std::shared_ptr<FileManager> fileManager;
    std::shared_ptr<CommandParser> commandParser;

public:
    Controller(const std::string& initPath = "");
    ~Controller();

    void setupBindings();
    std::string fileTimeToString(const std::filesystem::file_time_type& ftime);
    std::string formatSize(uintmax_t bytes);
    void parse(const std::string& inputLine);
};
