//
// Created by floppyhammer on 7/15/2021.
//

#ifndef DEMO_H
#define DEMO_H

#ifdef WIN32
    #include <glad/gl.h>
#elif defined(__ANDROID__)
    #include <GLES3/gl3.h>
#endif

#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>

// NanoVG
#include <nanovg/nanovg.h>

// NanoSVG
#include <nanosvg.h>

#ifdef __ANDROID__
    #include <android/log.h>
    #include <stdarg.h>
    #define LOG_TAG "nanovg_demo"
    #define XLOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
    #define XLOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#else
    #define XLOGI(...)
    #define XLOGE(...)
#endif

static void print_gl_string(const char* name, GLenum s) {
    const char* v = (const char*)glGetString(s);
    printf("GL %s = %s\n", name, v);
    XLOGI("GL %s = %s\n", name, v);
}

static void check_gl_error(const char* op) {
    for (GLint error = glGetError(); error; error = glGetError()) {
        printf("after %s() glError (0x%x)\n", op, error);
        XLOGE("after %s() glError (0x%x)\n", op, error);
    }
}

NVGpaint create_linear_gradient(NVGcontext* vg_context, NSVGgradient* gradient);

NVGpaint create_radial_gradient(NVGcontext* vg_context, NSVGgradient* gradient);

inline NVGcolor svg_color(unsigned int c) {
    return nvgRGBA((c & 0xFFu), ((c >> 8u) & 0xFFu), ((c >> 16u) & 0xFFu), ((c >> 24u) & 0xFFu));
}

class VgRenderer {
public:
    ~VgRenderer();

    // Screen size.
    int window_width{};
    int window_height{};

    NVGcontext* vg_context = nullptr;

    NSVGimage* vg_image = nullptr;

    bool setup_graphics(int p_width, int p_height, const char* svg_file_path, const char* font_file_path);

    void render_frame(float delta, float elapsed);
};

#endif // DEMO_H
