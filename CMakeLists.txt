cmake_minimum_required(VERSION 3.16.1)
project(nanovg-demo)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

include_directories(external third_party)

# GLFW library.
include_directories(external/glfw/include)
add_subdirectory(external/glfw)

add_executable(nanovg-demo
        demos/native/main.cpp
        external/glad.c
        third_party/nanovg/nanovg.c
        src/demo.h src/demo.cpp
        demos/native/window.h demos/native/window.cpp)

# Windows
target_link_libraries(nanovg-demo glfw)
