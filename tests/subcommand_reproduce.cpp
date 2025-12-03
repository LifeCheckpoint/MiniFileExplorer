#include <CLI/CLI.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

int main() {
    CLI::App app("MiniFileExplorer Shell");
    app.failure_message(CLI::FailureMessage::help);

    std::string temp_path_src;

    // cd
    auto cmd_cd = app.add_subcommand("cd", "Change directory");
    cmd_cd->add_option("path", temp_path_src, "Target path")->required();
    cmd_cd->callback([&]() {
        std::cout << "cd called with path: " << temp_path_src << std::endl;
    });

    std::cout << "Testing: cd /src/ (without shell_dummy, reversed)" << std::endl;
    std::vector<std::string> args = {"cd", "/src/"};
    std::reverse(args.begin(), args.end());

    try {
        app.parse(args);
    } catch (const CLI::CallForHelp& e) {
        std::cout << app.help() << std::endl;
    } catch (const CLI::ParseError& e) {
        std::cout << "Parse Error: " << e.what() << std::endl;
    }

    return 0;
}