//
// Created by floppyhammer on 7/15/2021.
//

#include "../../src/demo.h"
#include "window.h"

#include <chrono>

int main() {
    Window window(1920, 1080);

    VGRenderer vg_renderer;

    vg_renderer.setup_graphics(window.width, window.height,
                               "../res/images/tiger.svg",
                            "../res/fonts/lunchtime-doubly-so/lunchds.ttf");

    std::chrono::time_point<std::chrono::steady_clock> start_frame = std::chrono::steady_clock::now();
    auto last_frame = start_frame;
    auto last_time_show_fps = start_frame;

    // Rendering loop.
    while (!glfwWindowShouldClose(window.get_glfw_window())) {
        auto current_frame = std::chrono::steady_clock::now();

        // Time between frames in ms.
        std::chrono::duration<double> duration = current_frame - last_frame;
        auto delta = (float)round(duration.count() * 1000.0f);

        // Time since program started in ms.
        duration = current_frame - start_frame;
        auto elapsed = (float)round(duration.count() * 1000.0f);

        last_frame = current_frame;

        // Print frame time.
        duration = current_frame - last_time_show_fps;
        if (duration.count() > 2.0f) {
            printf("\nFrame time: %.1f ms.", delta);
            last_time_show_fps = current_frame;
        }

        // Handle input.
        window.handle_inputs();

        // Render frame.
        vg_renderer.render_frame(delta, elapsed);

        window.swap_buffers_and_poll_events();
    }

    return 0;
}
