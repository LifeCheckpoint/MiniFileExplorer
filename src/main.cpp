#include <iostream>
#include <string>
#include <vector>

#include "replxx.hxx"
#include <fmt/core.h>
#include <fmt/color.h>
#include "Controller.h"

int main() {
    replxx::Replxx rx;
    rx.install_window_change_handler();

    // auto-completion keywords
    std::vector<std::string> keywords = {"cd", "ls", "cp", "touch", "mkdir", "rm", "rmdir", "stat", "search", "du", "exit"};
    rx.set_completion_callback([&](std::string const& context, int& contextLen) {
        replxx::Replxx::completions_t completions;
        std::string prefix = context.substr(context.find_last_of(" \t") + 1);
        contextLen = prefix.length();
        for (auto const& kw : keywords) {
            if (kw.find(prefix) == 0) completions.emplace_back(kw);
        }
        return completions;
    });

    int line_count = 1;
    fmt::print(fg(fmt::color::yellow) | fmt::emphasis::bold, "Mini File Explorer Demo\n");
    fmt::print(fg(fmt::color::red) | fmt::emphasis::bold, "Created by LifeCheckpoint, LightningHonor.\n");

    Controller controller;

    while (true) {
        Path cur_path;
        controller.fileManager->getCurrentPath(cur_path);
        if (!cur_path.has_filename()) {
            cur_path = cur_path.parent_path();
        }
        std::string prompt = fmt::format("\033[1;34m[{}]\033[0m {}> ", line_count, cur_path.filename().string()); // Blue
        const char* input = rx.input(prompt);

        // EOF / ^D
        if (input == nullptr) break;
        
        std::string val(input);
        if (val.empty()) continue;
        
        rx.history_add(val);
        line_count++;

        if (val == "exit") break;
        
        // Parse commands
        std::string output;
        controller.parse(val, output);
        fmt::print("{}", output);
    }

    return 0;
}