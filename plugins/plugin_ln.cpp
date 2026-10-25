#define PLUGIN_BUILD
#include "../include/plugin_api.h"
#include <cmath>

extern "C" {
    PLUGIN_API const char* plugin_name() noexcept { return "ln"; }
    PLUGIN_API double plugin_eval(double arg) noexcept {
        if (arg <= 0.0) return std::nan("");  
        return std::log(arg);
    }
}