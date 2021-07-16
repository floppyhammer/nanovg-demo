//
// Created by chy on 6/25/2021.
//

#include "window.h"

#include <iostream>

Window::Window(int p_width, int p_height) : width(p_width), height(p_height) {
    init();
}

Window::~Window() {
    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
}

void Window::init() {
    glfw_window = init_glfw_window();
}

GLFWwindow* Window::init_glfw_window() const {
    // GLFW: initialize and configure.
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // GLFW: window creation.
    GLFWwindow* window = glfwCreateWindow(width, height, "", nullptr, nullptr);

    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // GLAD: load all OpenGL function pointers.
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }

    int glMajorVersion, glMinorVersion;
    glGetIntegerv(GL_MAJOR_VERSION, &glMajorVersion);
    glGetIntegerv(GL_MINOR_VERSION, &glMinorVersion);
    std::cout << "OpenGL version: " << glMajorVersion << '.' << glMinorVersion << std::endl;

    return window;
}

/// glfw: whenever the window size changed (by OS or user resize) this callback function executes.
void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

GLFWwindow* Window::get_glfw_window() {
    return glfw_window;
}

/// Process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly.
void Window::handle_inputs() {
    if (glfwGetKey(glfw_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(glfw_window, true);
}

void Window::swap_buffers_and_poll_events() {
    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.).
    glfwSwapBuffers(glfw_window);
    glfwPollEvents();
}
