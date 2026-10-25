#include "expression_parser.h"
#include "plugin_manager.h"

#include <iostream>
#include <string>

int main()
{
    PluginManager plugins;
    plugins.loadDirectory("./plugins");

    std::cout
        << "Calculator. Available without plugins: + - * / ()\n"
        << "Commands for testing plugins: :list (list of available plugins) | :load [path_of_plugin] | :unload [name_of_plugin] | exit\n";

    std::string line;
    while (true) {
        std::cout << ">>> ";
        if (!std::getline(std::cin, line)) break;
        if (line.empty()) continue;
        if (line == "exit" || line == "quit") break;

        if (line == ":list") {
            for (const auto& n : plugins.names())
                std::cout << "  " << n << '\n';
            continue;
        }

        if (line.rfind(":load ", 0) == 0) {
            plugins.loadFile(line.substr(6));
            continue;
        }

        if (line.rfind(":unload ", 0) == 0) {
            const std::string name = line.substr(8);
            if (plugins.unload(name)) {
                std::cerr << "[info] unloaded: " << name << '\n';
            }
            else {
                std::cerr << "[warn] not loaded: " << name << '\n';
            }
            continue;
        }

        try {
            ExpressionParser parser(line, plugins);
            std::cout << "= " << parser.parse() << '\n';
        }
        catch (const std::exception& e) {
            std::cout << "error: " << e.what() << '\n';
        }
        catch (...) {
            std::cout << "error: unknown exception\n";
        }
    }
    return 0;
}