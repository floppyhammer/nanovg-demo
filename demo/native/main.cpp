//
// Created by floppyhammer on 7/15/2021.
//

#include <chrono>

#include "../../src/demo.h"
#include "window.h"

int main() {
    Window window(1920, 1080);

    VgRenderer vg_renderer;

    vg_renderer.setup_graphics(window.width,
                               window.height,
                               "../assets/images/tiger.svg",
                               "../assets/fonts/lunchtime-doubly-so/lunchds.ttf");

    std::chrono::time_point<std::chrono::steady_clock> start_frame = std::chrono::steady_clock::now();
    auto last_frame = start_frame;
    auto last_time_show_fps = start_frame;

    // Rendering loop.
    while (!glfwWindowShouldClose(window.get_glfw_window())) {
        // Handle input.
        window.handle_inputs();

        // Render frame.
        vg_renderer.render_frame();

        window.swap_buffers_and_poll_events();
    }

    return 0;
}
