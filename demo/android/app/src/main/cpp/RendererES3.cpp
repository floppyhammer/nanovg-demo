#include "gles3jni.h"
#include <EGL/egl.h>

#include "demo.h"

class RendererES3 : public Renderer {
public:
    RendererES3();

    virtual ~RendererES3();

    bool init();

private:
    virtual float *mapOffsetBuf() { return nullptr; };

    virtual void unmapOffsetBuf() {};

    virtual float *mapTransformBuf() { return nullptr; };

    virtual void unmapTransformBuf() {};

    virtual void draw(unsigned int numInstances);

    const EGLContext mEglContext;

    VgRenderer vg_renderer;
};

Renderer *createES3Renderer() {
    auto *renderer = new RendererES3;
    if (!renderer->init()) {
        delete renderer;
        return nullptr;
    }
    return renderer;
}

RendererES3::RendererES3() : mEglContext(eglGetCurrentContext()) {}

bool RendererES3::init() {
    ALOGV("Using OpenGL ES 3.0 renderer");

    // You should prepare these assets on your phone first.
    vg_renderer.setup_graphics(1080, 1080,
                               "/sdcard/test/image.svg",
                               "/sdcard/test/font.ttf");

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
}

void RendererES3::draw(unsigned int numInstances) {
    vg_renderer.render_frame();
}
