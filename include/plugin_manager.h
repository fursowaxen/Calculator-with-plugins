#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#ifdef _WIN32
#include <windows.h>
struct ModuleDeleter {
    void operator()(HMODULE h) const noexcept { if (h) ::FreeLibrary(h); }
};
using ModuleHandle = std::unique_ptr<std::remove_pointer_t<HMODULE>, ModuleDeleter>;
#else
#include <dlfcn.h>
struct ModuleDeleter {
    void operator()(void* h) const noexcept { if (h) ::dlclose(h); }
};
using ModuleHandle = std::unique_ptr<void, ModuleDeleter>;
#endif

class PluginManager {
public:
    PluginManager() = default;
    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;

    void loadDirectory(const std::string& dir);
    bool loadFile(const std::string& path);
    bool unload(const std::string& name);
    std::optional<double> call(const std::string& name, double arg) const;
    std::vector<std::string> names() const;

private:
    struct Entry {
        ModuleHandle handle;
        double (*eval)(double) noexcept = nullptr;
    };
    std::unordered_map<std::string, Entry> plugins_;
};