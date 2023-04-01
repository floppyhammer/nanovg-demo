cmake_minimum_required(VERSION 3.16.1)

project(nanovg_demo)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

include_directories(third_party)

if (WIN32)
    include_directories(external)
    # GLFW library.
    include_directories(external/glfw/include)
    add_subdirectory(external/glfw)
endif ()

add_library(nanovg_demo src/demo.cpp third_party/nanovg/nanovg.c)

if (WIN32)
    add_subdirectory(demo/native)
endif ()
