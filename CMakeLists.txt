cmake_minimum_required(VERSION 3.20)
project(calc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Ядро
add_executable(calc
    src/main.cpp
    src/plugin_manager.cpp
    src/expression_parser.cpp
)

target_include_directories(calc PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (NOT WIN32)
    target_link_libraries(calc PRIVATE dl)
endif()

# Плагины 
function(add_calc_plugin name src)
    add_library(${name} MODULE ${src})
    target_include_directories(${name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    set_target_properties(${name} PROPERTIES
        PREFIX ""
        OUTPUT_NAME "${name}"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>/plugins"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>/plugins"
    )
    if (NOT WIN32)
        target_compile_options(${name} PRIVATE -fvisibility=hidden)
    endif()
endfunction()

add_calc_plugin(plugin_sin plugins/plugin_sin.cpp)
add_calc_plugin(plugin_cos plugins/plugin_cos.cpp)
add_calc_plugin(plugin_ln  plugins/plugin_ln.cpp)
add_calc_plugin(plugin_exp plugins/plugin_exp.cpp)

set_target_properties(calc PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>"
)