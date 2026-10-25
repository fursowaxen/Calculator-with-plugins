#pragma once

#ifdef _WIN32
#ifdef PLUGIN_BUILD
#define PLUGIN_API __declspec(dllexport)
#else
#define PLUGIN_API __declspec(dllimport)
#endif
#else
#define PLUGIN_API __attribute__((visibility("default")))
#endif

extern "C" {
    PLUGIN_API const char* plugin_name() noexcept;
    PLUGIN_API double plugin_eval(double arg) noexcept;
}