#define PLUGIN_BUILD
#include "../include/plugin_api.h"
#include <cmath>

extern "C" {
    PLUGIN_API const char* plugin_name() noexcept { return "cos"; }
    PLUGIN_API double plugin_eval(double arg) noexcept {
        return std::cos(arg * 3.14159265358979323846 / 180.0);
    }
} 