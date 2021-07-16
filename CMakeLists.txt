cmake_minimum_required(VERSION 3.16.1)
project(nanovg-demo)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

if(WIN32)
    include_directories(external/inc third_party)
    link_directories(external/lib/lib-vc2019)
else()
    include_directories(external/inc third_party)
endif()

add_executable(nanovg-demo
        src/main.cpp
        external/inc/glad.c
        third_party/nanovg/nanovg.c
        src/demo.h src/demo.cpp
        src/window.h src/window.cpp
        )

# Windows
if(WIN32)
    target_link_libraries(nanovg-demo glfw3)
else()
    # Linux
    # sudo apt install libglfw3-dev
    # -lglfw for glfw
    # -ldl for glad
    # -lGL for opengl
    target_link_libraries(flint -lglfw -ldl -lGL)
endif()
