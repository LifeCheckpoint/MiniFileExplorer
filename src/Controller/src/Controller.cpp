#include "Controller.h"
#include "FileManager.h"
#include "CommandParser.h"

#include <filesystem>
#include <chrono>
#include <tabulate/table.hpp>
#include <fmt/core.h>
#include <fmt/chrono.h>
#include <fmt/color.h>

using Path = std::filesystem::path;

Controller::Controller() {
    fileManager = std::make_shared<FileManager>();
    commandParser = std::make_shared<CommandParser>();
    setupBindings();
}

Controller::~Controller() = default;

void Controller::setupBindings()
{
    commandParser->onChangeDirectory = [this](const std::string& targetDirectory) {
        Status status = fileManager->changeDirectory(targetDirectory);
        outputMsgCur = status.ok() ? "" : status.message + "\n";
    };

    commandParser->onListFiles = [this](bool sortSize, bool sortTime) {
        SortMode sortMode = SortMode::Default;
        if (sortSize) sortMode = SortMode::BySize;
        else if (sortTime) sortMode = SortMode::ByTime;

        std::vector<FileInfo> files;
        Status status = fileManager->listFiles(sortMode, files);
        if (status.ok()) {
            std::string output;
            tabulate::Table fileTable;
            fileTable.add_row({"Name", "Type", "Size (bytes)", "Last Modified"});

            for (const auto& file : files) {
                fileTable.add_row({
                    file.name,
                    (file.type == FileType::Directory) ? "Directory" : (file.type == FileType::File) ? "File" : "Unknown",
                    (file.type == FileType::Directory) ? "" : std::to_string(file.size),
                    fileTimeToString(file.modifyTime)
                });
            }
            fileTable.format()
                     .font_style({tabulate::FontStyle::bold})
                     .border_top(" ")
                     .border_bottom(" ")
                     .border_left(" ")
                     .border_right(" ")
                     .corner(" ");
            fileTable[0].format()
                        .padding_top(1)
                        .padding_bottom(1)
                        .font_align(tabulate::FontAlign::center)
                        .font_style({tabulate::FontStyle::underline})
                        .font_background_color(tabulate::Color::red);
            fileTable.column(0)
                     .format()
                     .font_color(tabulate::Color::yellow);
            fileTable.print(std::cout);
            outputMsgCur = output;
        } else {
            outputMsgCur = status.message + "\n";
        }
    };

    commandParser->onCopy = [this](const std::string& sourcePath, const std::string& targetPath) {
        Status status = fileManager->copyItem(sourcePath, targetPath);
        outputMsgCur = status.ok() ? "" : status.message + "\n";
    };

    commandParser->onTouchFile = [this](const std::string& path) {
        Status status = fileManager->createFile(path);
        outputMsgCur = status.ok() ? "" : status.message + "\n";
    };

    commandParser->onMakeDirectory = [this](const std::string& path) {
        Status status = fileManager->createDirectory(path);
        outputMsgCur = status.ok() ? "" : status.message + "\n";
    };

    commandParser->onRemove = [this](const std::string& path) {
        Status status = fileManager->removePath(path);
        outputMsgCur = status.ok() ? "" : status.message + "\n";
    };

    commandParser->onRemoveDirectory = [this](const std::string& path) {
        Status status = fileManager->removePath(path);
        outputMsgCur = status.ok() ? "" : status.message + "\n";
    };

    commandParser->onStat = [this](const std::string& path) {
        FileInfo info;
        Status status = fileManager->getFileStat(path, info);
        if (status.ok()) {
            std::string output = fmt::format(
                "Name: {}\nType: {}\nSize: {} bytes\nLast Modified: {}",
                info.name,
                (info.type == FileType::Directory) ? "Directory" : (info.type == FileType::File) ? "File" : "Unknown",
                info.size,
                fileTimeToString(info.modifyTime)
            );
            outputMsgCur = output + "\n";
        } else {
            outputMsgCur = status.message + "\n";
        }
    };

    commandParser->onExit = [this]() {
        outputMsgCur = "Exiting shell...\n";
    };
}

void Controller::parse(const std::string& inputLine, std::string& outputMessage) {
    commandParser->process(inputLine);
    outputMessage = outputMsgCur;
}

std::string Controller::fileTimeToString(const std::filesystem::file_time_type& ftime) {
    auto sys_time = std::chrono::file_clock::to_sys(ftime);
    auto local_time = std::chrono::zoned_time{std::chrono::current_zone(), sys_time};
    return fmt::format("{:%Y-%m-%d %H:%M:%S}\n", local_time);
}