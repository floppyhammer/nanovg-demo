//
// Created by chy on 6/25/2021.
//

#ifndef WINDOW_H
#define WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window {
public:
    int width, height;

    Window(int p_width, int p_height);

    ~Window();

    void init();

    GLFWwindow* get_glfw_window();

    void handle_inputs();

    void swap_buffers_and_poll_events();

private:
    GLFWwindow* glfw_window = nullptr;

    GLFWwindow* init_glfw_window() const;

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
};

#endif //WINDOW_H
