#include "plugin_manager.h"
#include "plugin_api.h"

#include <cmath>
#include <filesystem>
#include <iostream>

#ifdef _WIN32
static HMODULE loadLib(const std::string& p) { return ::LoadLibraryA(p.c_str()); }
static void* getSym(HMODULE h, const char* n) { return (void*)::GetProcAddress(h, n); }
#else
static void* loadLib(const std::string& p) { return ::dlopen(p.c_str(), RTLD_NOW | RTLD_LOCAL); }
static void* getSym(void* h, const char* n) { return ::dlsym(h, n); }
#endif

void PluginManager::loadDirectory(const std::string& dir) {
    namespace fs = std::filesystem;
    std::error_code ec;

    if (!fs::is_directory(dir, ec)) {
        std::cerr << "[info] no plugins directory: " << dir << '\n';
        return;
    }

    for (const auto& entry : fs::directory_iterator(dir, ec)) {
        if (ec) {
            std::cerr << "[warn] " << ec.message() << '\n';
            break;
        }
        if (!entry.is_regular_file()) continue;
        const auto ext = entry.path().extension().string();
#ifdef _WIN32
        if (ext != ".dll") continue;
#else
        if (ext != ".so")  continue;
#endif
        loadFile(entry.path().string());
    }
}

bool PluginManager::loadFile(const std::string& path) {ModuleHandle handle(loadLib(path));
    if (!handle) {
        std::cerr << "[warn] failed to load " << path << '\n';
        return false;
    }

    auto nameFn = reinterpret_cast<const char* (*)() noexcept>(getSym(handle.get(), "plugin_name"));
    auto evalFn = reinterpret_cast<double(*)(double) noexcept>(getSym(handle.get(), "plugin_eval"));

    if (!nameFn || !evalFn) {
        std::cerr << "[warn] " << path << ": missing exports\n";
        return false;
    }

    const char* name = nameFn();
    if (!name || !*name) {
        std::cerr << "[warn] " << path << ": empty plugin name\n";
        return false;
    }
    if (plugins_.count(name)) {
        std::cerr << "[warn] " << path << ": duplicate name '" << name << "'\n";
        return false;
    }

    plugins_.emplace(std::string(name), Entry{ std::move(handle), evalFn });
    std::cerr << "[info] loaded: " << name << '\n';
    return true;
}

bool PluginManager::unload(const std::string& name) {
    return plugins_.erase(name) != 0;
}

std::optional<double> PluginManager::call(const std::string& name, double arg) const {
    auto it = plugins_.find(name);
    if (it == plugins_.end()) return std::nullopt;

    double r = it->second.eval(arg);
    if (std::isnan(r)) return std::nullopt;
    return r;
}

std::vector<std::string> PluginManager::names() const {
    std::vector<std::string> out;
    out.reserve(plugins_.size());
    for (const auto& [name, _] : plugins_) out.push_back(name);
    return out;
}