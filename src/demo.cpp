//
// Created by floppyhammer on 7/15/2021.
//

#include "demo.h"

// NanoVG
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

// STB Image
#define STB_IMAGE_IMPLEMENTATION
#include <nanovg/stb_image.h>

/*
When used in Android demo, make sure to set EGL_DEPTH_SIZE = 16 and EGL_STENCIL_SIZE = 8
with setEGLConfigChooser() in JNIView.java.
*/

bool VgRenderer::setup_graphics(int p_width, int p_height, const char *svg_file_path, const char *font_file_path) {
    print_gl_string("Version", GL_VERSION);
    print_gl_string("Vendor", GL_VENDOR);
    print_gl_string("Renderer", GL_RENDERER);
    print_gl_string("Extensions", GL_EXTENSIONS);

    window_width = p_width;
    window_height = p_height;

    // NanoVG: Load SVG.
    vg_image = nsvgParseFromFile(svg_file_path, "px", 96.0f);
    if (vg_image == nullptr) {
        printf("Failed to open SVG file!\n");
        XLOGE("Failed to open SVG file!\n");
        return false;
    }

    // NanoVG: Create context. (Disabled NVG_DEBUG for benchmark.)
#ifdef WIN32
    vg_context = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
#elif defined(__ANDROID__)
    vg_context = nvgCreateGLES3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
#endif

    // NanoVG: Create font.
    int res = nvgCreateFont(vg_context, "default", font_file_path);
    if (res < 0) {
        printf("Failed to create font, error %d\n", res);
        XLOGE("Failed to create font, error %d\n", res);
    }

    return true;
}

NVGpaint create_linear_gradient(NVGcontext *vg_context, NSVGgradient *gradient) {
    float inverse[6];
    float sx, sy, ex, ey;

    nvgTransformInverse(inverse, gradient->xform);
    nvgTransformPoint(&sx, &sy, inverse, 0, 0);
    nvgTransformPoint(&ex, &ey, inverse, 0, 1);

    return nvgLinearGradient(vg_context,
                             sx,
                             sy,
                             ex,
                             ey,
                             svg_color(gradient->stops[0].color),
                             svg_color(gradient->stops[gradient->nstops - 1].color));
}

NVGpaint create_radial_gradient(NVGcontext *vg_context, NSVGgradient *gradient) {
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

    NVGpaint paint = nvgRadialGradient(vg_context,
                                       cx,
                                       cy,
                                       inr,
                                       outr,
                                       svg_color(gradient->stops[0].color),
                                       svg_color(gradient->stops[gradient->nstops - 1].color));

    return paint;
}

void VgRenderer::render_frame(float delta, float elapsed) {
    // Render to screen.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, window_width, window_height);

    // Clear framebuffer.
    glClearColor(0.2, 0.2, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // NanoVG: Begin.
    nvgBeginFrame(vg_context, window_width, window_height, 1);

    // Transform.
    nvgResetTransform(vg_context);
    float scale_factor = ((float)std::sin(elapsed * 0.001f) + 1.0f) * 2.0f;
    scale_factor = 1.0f;
    nvgTranslate(vg_context,
                 (float)window_width / 2 - vg_image->width * scale_factor / 2,
                 (float)window_height / 2 - vg_image->height * scale_factor / 2);
    nvgScale(vg_context, scale_factor, scale_factor);

    // Traverse the SVG tree.
    for (auto *shape = vg_image->shapes; shape != nullptr; shape = shape->next) {
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
        nvgBeginPath(vg_context);

        for (auto *path = shape->paths; path != nullptr; path = path->next) {
            nvgMoveTo(vg_context, path->pts[0], path->pts[1]);

            // Traverse Bezier curves.
            for (int i = 0; i < path->npts - 3; i += 3) {
                float *p = &path->pts[i * 2];
                nvgBezierTo(vg_context, p[2], p[3], p[4], p[5], p[6], p[7]);
            }

            if (path->closed) {
                nvgLineTo(vg_context, path->pts[0], path->pts[1]);
            }

            // Draw fill.
            switch (shape->fill.type) {
                case NSVG_PAINT_COLOR:
                    nvgFillColor(vg_context, nvgRGBA(fill_cr, fill_cg, fill_cb, fill_ca));
                    nvgFill(vg_context);
                    break;
                case NSVG_PAINT_LINEAR_GRADIENT:
                    nvgFillPaint(vg_context, create_linear_gradient(vg_context, shape->fill.gradient));
                    nvgFill(vg_context);
                    break;
                case NSVG_PAINT_RADIAL_GRADIENT:
                    nvgFillPaint(vg_context, create_radial_gradient(vg_context, shape->fill.gradient));
                    nvgFill(vg_context);
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
            nvgLineJoin(vg_context, join);

            // NanoSVG has the same line cap constants values as NanoVG.
            nvgLineCap(vg_context, shape->strokeLineCap);
            nvgStrokeWidth(vg_context, shape->strokeWidth);

            // Draw stroke.
            switch (shape->stroke.type) {
                case NSVG_PAINT_COLOR: {
                    nvgStrokeColor(vg_context, nvgRGBA(stroke_cr, stroke_cg, stroke_cb, stroke_ca));
                    nvgStroke(vg_context);
                } break;
                case NSVG_PAINT_LINEAR_GRADIENT: {
                    nvgStrokePaint(vg_context, create_linear_gradient(vg_context, shape->stroke.gradient));
                    nvgStroke(vg_context);
                } break;
                case NSVG_PAINT_RADIAL_GRADIENT: {
                    nvgStrokePaint(vg_context, create_radial_gradient(vg_context, shape->stroke.gradient));
                    nvgStroke(vg_context);
                } break;
            }
        }

        nvgClosePath(vg_context);
    }

    // Render text.
    // --------------------------------------
    // Set text buffer.
    char buffer[64];
    memset(&buffer[0], 0, sizeof(buffer));
    int count = snprintf(buffer, sizeof buffer, "%.1f", delta);

    // printf("Number of bytes that are written in the array: %d\n", count);
    // XLOGI("Number of bytes that are written in the array: %d\n", count);

    buffer[count] = ' ';
    buffer[count + 1] = 'm';
    buffer[count + 2] = 's';

    // Reset transform for text.
    nvgResetTransform(vg_context);
    nvgFontSize(vg_context, 48.0f);
    nvgFontFace(vg_context, "default");
    nvgFillColor(vg_context, nvgRGBA(255, 255, 255, 255));
    nvgTextAlign(vg_context, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
    nvgText(vg_context, 0, 0, buffer, nullptr);
    // --------------------------------------

    // NanoVG: End.
    nvgEndFrame(vg_context);
}

VgRenderer::~VgRenderer() {
#ifdef WIN32
    nvgDeleteGL3(vg_context);
#elif defined(__ANDROID__)
    nvgDeleteGLES3(vg_context);
#endif
}
