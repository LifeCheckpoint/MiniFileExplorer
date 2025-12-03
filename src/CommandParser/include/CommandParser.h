#pragma once
#include <CLI/CLI.hpp>
#include <string>
#include <vector>
#include <functional>
#include <iostream>

class CommandParser {
public:
    // Hooks for commands

    // cd
    std::function<void(const std::string &targetDirectory)> onChangeDirectory;
    
    // ls
    std::function<void(bool sortSize, bool sortTime)> onListFiles;
    
    // cp
    std::function<void(const std::string &sourcePath, const std::string &targetPath)> onCopy;
    
    // touch
    std::function<void(const std::string &path)> onTouchFile;

    // mkdir
    std::function<void(const std::string &path)> onMakeDirectory;

    // rm
    std::function<void(const std::string &path)> onRemove;

    // rmdir
    std::function<void(const std::string &path)> onRemoveDirectory;

    // stat
    std::function<void(const std::string &path)> onStat;

    // search
    std::function<void(const std::string &keyword)> onSearch;

    // du
    std::function<void(const std::string &path)> onDiskUsage;

    // exit
    std::function<void()> onExit;

public:
    CommandParser() : app("MiniFileExplorer Shell") {
        setupCLI();
    }

    // Process single line command
    void process(const std::string& inputLine) {
        if (inputLine.empty()) return;

        std::vector<std::string> args = CLI::detail::split_up(inputLine);
        
        // Use a dummy program name to avoid parse command to program name confusion
        args.insert(args.begin(), "shell_dummy");

        try {
            app.parse(args);
        } catch (const CLI::CallForHelp& e) {
            std::cout << app.help() << std::endl;
        } catch (const CLI::ParseError& e) {
            app.exit(e);
        }
        
        app.clear();
    }

private:
    CLI::App app;

    std::string temp_path_src;
    std::string temp_path_dst;
    bool temp_flag_size = false;
    bool temp_flag_time = false;

    void setupCLI() {
        app.failure_message(CLI::FailureMessage::help);

        // cd
        auto cmd_cd = app.add_subcommand("cd", "Change directory");
        cmd_cd->add_option("path", temp_path_src, "Target path")->required();
        cmd_cd->callback([this]() {
            if (onChangeDirectory) onChangeDirectory(temp_path_src);
        });

        // ls
        auto cmd_ls = app.add_subcommand("ls", "List files");
        cmd_ls->add_flag("-s", temp_flag_size, "Sort by size");
        cmd_ls->add_flag("-t", temp_flag_time, "Sort by time");
        cmd_ls->callback([this]() {
            if (onListFiles) onListFiles(temp_flag_size, temp_flag_time);
        });

        // cp
        auto cmd_cp = app.add_subcommand("cp", "Copy file");
        cmd_cp->add_option("src", temp_path_src, "Source")->required();
        cmd_cp->add_option("dst", temp_path_dst, "Dest")->required();
        cmd_cp->callback([this]() {
            if (onCopy) onCopy(temp_path_src, temp_path_dst);
        });

        // touch
        auto cmd_touch = app.add_subcommand("touch", "Create empty file");
        cmd_touch->add_option("path", temp_path_src, "File path")->required();
        cmd_touch->callback([this]() {
            if (onTouchFile) onTouchFile(temp_path_src);
        });

        // mkdir
        auto cmd_mkdir = app.add_subcommand("mkdir", "Create directory");
        cmd_mkdir->add_option("path", temp_path_src, "Directory path")->required();
        cmd_mkdir->callback([this]() {
            if (onMakeDirectory) onMakeDirectory(temp_path_src);
        });

        // rm
        auto cmd_rm = app.add_subcommand("rm", "Remove file");
        cmd_rm->add_option("path", temp_path_src, "File path")->required();
        cmd_rm->callback([this]() {
            if (onRemove) onRemove(temp_path_src);
        });

        // rmdir
        auto cmd_rmdir = app.add_subcommand("rmdir", "Remove directory");
        cmd_rmdir->add_option("path", temp_path_src, "Directory path")->required();
        cmd_rmdir->callback([this]() {
            if (onRemoveDirectory) onRemoveDirectory(temp_path_src);
        });

        // stat
        auto cmd_stat = app.add_subcommand("stat", "Show file status");
        cmd_stat->add_option("path", temp_path_src, "Path")->required();
        cmd_stat->callback([this]() {
            if (onStat) onStat(temp_path_src);
        });

        // search
        auto cmd_search = app.add_subcommand("search", "Search files");
        cmd_search->add_option("keyword", temp_path_src, "Keyword")->required();
        cmd_search->callback([this]() {
            if (onSearch) onSearch(temp_path_src);
        });

        // du
        auto cmd_du = app.add_subcommand("du", "Disk usage");
        cmd_du->add_option("path", temp_path_src, "Path")->required();
        cmd_du->callback([this]() {
            if (onDiskUsage) onDiskUsage(temp_path_src);
        });

        // exit
        app.add_subcommand("exit", "Exit shell")->callback([this](){
            if (onExit) onExit();
        });
        }
    };
