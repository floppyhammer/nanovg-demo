add_executable(nanovg_demo_native
        main.cpp
        window.cpp
        )

target_link_libraries(nanovg_demo_native nanovg_demo glfw)
