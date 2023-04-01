//
// Created by floppyhammer on 7/15/2021.
//

#include "demo.h"

#ifdef WIN32
    #define NANOVG_GL3_IMPLEMENTATION
#elif defined(__ANDROID__)
    #define NANOVG_GLES3_IMPLEMENTATION
#endif

#include <nanovg/nanovg.h>
#include <nanovg/nanovg_gl.h>
#include <nanovg/nanovg_gl_utils.h>

#define NANOSVG_IMPLEMENTATION
#include <nanosvg.h>

#define STB_IMAGE_IMPLEMENTATION
#include <nanovg/stb_image.h>

/// When used in Android demo, make sure to set EGL_DEPTH_SIZE = 16 and EGL_STENCIL_SIZE = 8
/// with setEGLConfigChooser() in JNIView.java.

VgRenderer::VgRenderer() {
    start_time = std::chrono::steady_clock::now();
    last_frame_time = start_time;
    last_time_show_fps = start_time;
}

VgRenderer::~VgRenderer() {
#ifdef WIN32
    nvgDeleteGL3(nvg_context);
#elif defined(__ANDROID__)
    nvgDeleteGLES3(nvg_context);
#endif
}

bool VgRenderer::setup_graphics(int _canvas_width,
                                int _canvas_height,
                                const char *svg_file_path,
                                const char *font_file_path) {
    print_gl_string("Version", GL_VERSION);
    print_gl_string("Vendor", GL_VENDOR);
    print_gl_string("Renderer", GL_RENDERER);
    print_gl_string("Extensions", GL_EXTENSIONS);

    canvas_width = _canvas_width;
    canvas_height = _canvas_height;

    // NanoVG: Load SVG.
    nsvg_image = nsvgParseFromFile(svg_file_path, "px", 96.0f);
    if (nsvg_image == nullptr) {
        printf("Failed to open SVG file!\n");
        XLOGE("Failed to open SVG file!\n");
        return false;
    }

    // NanoVG: Create context. (Disabled NVG_DEBUG for benchmark.)
#ifdef WIN32
    nvg_context = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
#elif defined(__ANDROID__)
    nvg_context = nvgCreateGLES3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
#endif

    // NanoVG: Create font.
    int res = nvgCreateFont(nvg_context, "default", font_file_path);
    if (res < 0) {
        printf("Failed to create font, error %d\n", res);
        XLOGE("Failed to create font, error %d\n", res);
    }

    return true;
}

NVGpaint create_linear_gradient(NVGcontext *nvg_context, NSVGgradient *gradient) {
    float inverse[6];
    float sx, sy, ex, ey;

    nvgTransformInverse(inverse, gradient->xform);
    nvgTransformPoint(&sx, &sy, inverse, 0, 0);
    nvgTransformPoint(&ex, &ey, inverse, 0, 1);

    return nvgLinearGradient(nvg_context,
                             sx,
                             sy,
                             ex,
                             ey,
                             svg_color(gradient->stops[0].color),
                             svg_color(gradient->stops[gradient->nstops - 1].color));
}

NVGpaint create_radial_gradient(NVGcontext *nvg_context, NSVGgradient *gradient) {
    float inverse[6];
    float cx, cy, r1, r2, inr, outr;

    nvgTransformInverse(inverse, gradient->xform);
    nvgTransformPoint(&cx, &cy, inverse, 0, 0);
    nvgTransformPoint(&r1, &r2, inverse, 0, 1);
    outr = r2 - cy;
    if (gradient->nstops == 3)
        inr = gradient->stops[1].offset * outr;
    else
        inr = 0;

    NVGpaint paint = nvgRadialGradient(nvg_context,
                                       cx,
                                       cy,
                                       inr,
                                       outr,
                                       svg_color(gradient->stops[0].color),
                                       svg_color(gradient->stops[gradient->nstops - 1].color));

    return paint;
}

void VgRenderer::render_frame() {
    // Render to screen.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, canvas_width, canvas_height);

    // Clear framebuffer.
    glClearColor(0.2, 0.2, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // NanoVG: Begin.
    nvgBeginFrame(nvg_context, canvas_width, canvas_height, 1);

    // Transform.
    nvgResetTransform(nvg_context);
    nvgTranslate(nvg_context, 0, 0);
    nvgScale(nvg_context, 1, 1);

    // Traverse the SVG tree.
    for (auto *shape = nsvg_image->shapes; shape != nullptr; shape = shape->next) {
        if (!(shape->flags & NSVG_FLAGS_VISIBLE)) continue;

        unsigned int fill_cr = shape->fill.color & 0xffu;
        unsigned int fill_cg = (shape->fill.color >> 8u) & 0xffu;
        unsigned int fill_cb = (shape->fill.color >> 16u) & 0xffu;
        unsigned int fill_ca = (shape->fill.color >> 24u) & 0xffu;

        unsigned int stroke_cr = shape->stroke.color & 0xffu;
        unsigned int stroke_cg = (shape->stroke.color >> 8u) & 0xffu;
        unsigned int stroke_cb = (shape->stroke.color >> 16u) & 0xffu;
        unsigned int stroke_ca = (shape->stroke.color >> 24u) & 0xffu;

        // New path.
        nvgBeginPath(nvg_context);

        for (auto *path = shape->paths; path != nullptr; path = path->next) {
            nvgMoveTo(nvg_context, path->pts[0], path->pts[1]);

            // Traverse Bezier curves.
            for (int i = 0; i < path->npts - 3; i += 3) {
                float *p = &path->pts[i * 2];
                nvgBezierTo(nvg_context, p[2], p[3], p[4], p[5], p[6], p[7]);
            }

            if (path->closed) {
                nvgLineTo(nvg_context, path->pts[0], path->pts[1]);
            }

            // Draw fill.
            switch (shape->fill.type) {
                case NSVG_PAINT_COLOR:
                    nvgFillColor(nvg_context, nvgRGBA(fill_cr, fill_cg, fill_cb, fill_ca));
                    nvgFill(nvg_context);
                    break;
                case NSVG_PAINT_LINEAR_GRADIENT:
                    nvgFillPaint(nvg_context, create_linear_gradient(nvg_context, shape->fill.gradient));
                    nvgFill(nvg_context);
                    break;
                case NSVG_PAINT_RADIAL_GRADIENT:
                    nvgFillPaint(nvg_context, create_radial_gradient(nvg_context, shape->fill.gradient));
                    nvgFill(nvg_context);
                    break;
            }

            // Set stroke parameters.
            int join = NVG_MITER; // Default
            switch (shape->strokeLineJoin) {
                case NSVG_JOIN_ROUND:
                    join = NVG_ROUND;
                    break;
                case NSVG_JOIN_BEVEL:
                    join = NVG_BEVEL;
                    break;
            }
            nvgLineJoin(nvg_context, join);

            // NanoSVG has the same line cap constants values as NanoVG.
            nvgLineCap(nvg_context, shape->strokeLineCap);
            nvgStrokeWidth(nvg_context, shape->strokeWidth);

            // Draw stroke.
            switch (shape->stroke.type) {
                case NSVG_PAINT_COLOR: {
                    nvgStrokeColor(nvg_context, nvgRGBA(stroke_cr, stroke_cg, stroke_cb, stroke_ca));
                    nvgStroke(nvg_context);
                } break;
                case NSVG_PAINT_LINEAR_GRADIENT: {
                    nvgStrokePaint(nvg_context, create_linear_gradient(nvg_context, shape->stroke.gradient));
                    nvgStroke(nvg_context);
                } break;
                case NSVG_PAINT_RADIAL_GRADIENT: {
                    nvgStrokePaint(nvg_context, create_radial_gradient(nvg_context, shape->stroke.gradient));
                    nvgStroke(nvg_context);
                } break;
            }
        }

        nvgClosePath(nvg_context);
    }

    auto current_time = std::chrono::steady_clock::now();
    std::chrono::duration<double> duration = current_time - last_frame_time;
    auto dt_in_ms = duration.count() * 1000.0f;

    // Print frame time.
    duration = current_time - last_time_show_fps;
    if (duration.count() > 2.0f) {
        XLOGI("\nFrame time: %.1f ms.", dt_in_ms);
        last_time_show_fps = current_time;
    }

    last_frame_time = current_time;

    // Render text.
    // --------------------------------------
    // Reset transform for text.
    nvgResetTransform(nvg_context);
    nvgFontSize(nvg_context, 48.0f);
    nvgFontFace(nvg_context, "default");
    nvgFillColor(nvg_context, nvgRGBA(255, 255, 255, 255));
    nvgTextAlign(nvg_context, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
    nvgText(nvg_context, 0, 0, "Test", nullptr);
    // --------------------------------------

    // NanoVG: End.
    nvgEndFrame(nvg_context);
}
