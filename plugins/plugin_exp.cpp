#define PLUGIN_BUILD
#include "../include/plugin_api.h"
#include <cmath>

extern "C" {
    PLUGIN_API const char* plugin_name() noexcept { return "exp"; }
    PLUGIN_API double plugin_eval(double arg) noexcept {
        return std::exp(arg);
    }
}