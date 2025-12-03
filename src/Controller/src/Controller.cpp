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

std::string Controller::outputMsgCur;

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

    commandParser->onSearch = [this](const std::string& keyword) {
        std::vector<FileInfo> results;
        Status status = fileManager->search(keyword, results);
        if (status.ok()) {
            std::string output;
            for (const auto& file : results) {
                output += fmt::format("{}\n", file.path.string());
            }
            if (output.empty()) output = "No files found.\n";
            outputMsgCur = output;
        } else {
            outputMsgCur = status.message + "\n";
        }
    };

    commandParser->onDiskUsage = [this](const std::string& path) {
        uintmax_t size;
        Status status = fileManager->calculateDirSize(path, size);
        if (status.ok()) {
            outputMsgCur = fmt::format("Size: {} bytes\n", size);
        } else {
            outputMsgCur = status.message + "\n";
        }
    };

    commandParser->onExit = [this]() {
        outputMsgCur = "Exiting shell...\n";
    };
}

void Controller::parse(const std::string& inputLine, std::string& outputMessage) {
    outputMsgCur = ""; // Clear previous output
    commandParser->process(inputLine);
    outputMessage = outputMsgCur;
}

std::string Controller::fileTimeToString(const std::filesystem::file_time_type& ftime) {
    auto sys_time = std::chrono::file_clock::to_sys(ftime);
    auto time_t_val = std::chrono::system_clock::to_time_t(sys_time);
    auto local_tm = *std::localtime(&time_t_val);
    return fmt::format("{:04d}-{:02d}-{:02d} {:02d}:{:02d}:{:02d}\n",
                       local_tm.tm_year + 1900,
                       local_tm.tm_mon + 1,
                       local_tm.tm_mday,
                       local_tm.tm_hour,
                       local_tm.tm_min,
                       local_tm.tm_sec);
}