/*
 * Copyright 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gles3jni.h"
#include <EGL/egl.h>

#include "src/demo.h"

class RendererES3: public Renderer {
public:
    RendererES3();
    virtual ~RendererES3();
    bool init();

    /////////////////// YOUR CODE ///////////////////
    std::chrono::time_point<std::chrono::steady_clock> start_frame, last_frame, last_time_show_fps;
    VgRenderer vg_renderer;
    /////////////////// YOUR CODE ///////////////////

private:
    virtual float* mapOffsetBuf() { return nullptr; };
    virtual void unmapOffsetBuf() { };
    virtual float* mapTransformBuf() { return nullptr; };
    virtual void unmapTransformBuf() { };
    virtual void draw(unsigned int numInstances);

    const EGLContext mEglContext;
};

Renderer* createES3Renderer() {
    auto* renderer = new RendererES3;
    if (!renderer->init()) {
        delete renderer;
        return nullptr;
    }
    return renderer;
}

RendererES3::RendererES3() : mEglContext(eglGetCurrentContext()) { }

bool RendererES3::init() {
    ALOGV("Using OpenGL ES 3.0 renderer");

    /////////////////// YOUR CODE ///////////////////
    start_frame = std::chrono::steady_clock::now();
    last_frame = start_frame;
    last_time_show_fps = start_frame;

    auto window_width = 1080;
    auto window_height = 1920;

    vg_renderer.setup_graphics(window_width, window_height,
                            "/sdcard/test/images/tiger.svg",
                            "/sdcard/test/fonts/lunchtime-doubly-so/lunchds.ttf");
    /////////////////// YOUR CODE ///////////////////

    return true;
}

RendererES3::~RendererES3() {
    /* The destructor may be called after the context has already been
     * destroyed, in which case our objects have already been destroyed.
     *
     * If the context exists, it must be current. This only happens when we're
     * cleaning up after a failed init().
     */
    if (eglGetCurrentContext() != mEglContext)
        return;

    /////////////////// YOUR CODE ///////////////////

    /////////////////// YOUR CODE ///////////////////
}

void RendererES3::draw(unsigned int numInstances) {
    /////////////////// YOUR CODE ///////////////////
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
        XLOGI("\nFrame time: %.1f ms.", delta);
        last_time_show_fps = current_frame;
    }

    // Render frame.
    vg_renderer.render_frame(delta, elapsed);
    /////////////////// YOUR CODE ///////////////////
}
