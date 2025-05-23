#ifndef MT_RENDER_HPP_
#define MT_RENDER_HPP_

#include <cstring>
#include <glm/ext/vector_float4.hpp>
#include <glm/vec3.hpp>
#include <utility>

#include "barycentric.hpp"
#include "buffers/buffer.hpp"
#include "buffers/framebuffer.hpp"
#include "buffers/ib.hpp"
#include "mtdefs.hpp"

#define _MT_RASTERIZE_LOOP                                                                         \
    do {                                                                                           \
        for (u32 y = u32(a.y), row = y * m_width; y < u32(c.y); ++y, row += m_width) {             \
                                                                                                   \
            f32 w  = w0;                                                                           \
            f32 b0 = b00;                                                                          \
            f32 b1 = b10;                                                                          \
            f32 b2 = b20;                                                                          \
                                                                                                   \
            for (u32 x = u32(x0 - 0.5f); x < u32(x1); ++x) {                                       \
                                                                                                   \
                frag.barycoord = glm::vec3(b0, b1, b2) * w;                                        \
                frag.loc       = row + x;                                                          \
                frag(m_color[frag.loc]);                                                           \
                                                                                                   \
                w += slope_w0;                                                                     \
                b0 += slope_b00;                                                                   \
                b1 += slope_b10;                                                                   \
                b2 += slope_b20;                                                                   \
            }                                                                                      \
                                                                                                   \
            x0 += slope_x0;                                                                        \
            x1 += slope_x1;                                                                        \
            w0 += slope_w1;                                                                        \
                                                                                                   \
            b00 += slope_b01;                                                                      \
            b10 += slope_b11;                                                                      \
            b20 += slope_b21;                                                                      \
        }                                                                                          \
    } while (0)

namespace mt {

    void Framebuffer::RenderTop(
        const glm::vec4& a, const glm::vec4& b, const glm::vec4& c, FragShaderBase& frag,
        const Barycentric& bary
    ) {

        f32 slope_x0 = (b.x - a.x) / (b.y - a.y);
        f32 slope_x1 = (c.x - a.x) / (c.y - a.y);
        f32 x0 = a.x, x1 = a.x;

        f32 slope_w0                           = (c.w - b.w) / (c.x - b.x);
        f32 slope_w1                           = (b.w - a.w) / (b.y - a.y);
        f32 w0                                 = a.w;

        auto [slope_b00, slope_b10, slope_b20] = bary.GetDeltaX();
        auto [slope_b01, slope_b11, slope_b21] = bary.GetDeltaY(slope_x0);
        auto [b00, b10, b20]                   = bary.GetCoord(a.x, a.y);

        _MT_RASTERIZE_LOOP;
    }

    void Framebuffer::RenderBottom(
        const glm::vec4& a, const glm::vec4& b, const glm::vec4& c, FragShaderBase& frag,
        const Barycentric& bary
    ) {

        f32 slope_x0 = (c.x - a.x) / (c.y - a.y);
        f32 slope_x1 = (c.x - b.x) / (c.y - b.y);
        f32 x0 = a.x, x1 = b.x;

        f32 slope_w0                           = (b.w - a.w) / (b.x - a.x);
        f32 slope_w1                           = (c.w - a.w) / (c.y - a.y);
        f32 w0                                 = a.w;

        auto [slope_b00, slope_b10, slope_b20] = bary.GetDeltaX();
        auto [slope_b01, slope_b11, slope_b21] = bary.GetDeltaY(slope_x0);
        auto [b00, b10, b20]                   = bary.GetCoord(a.x, a.y);

        _MT_RASTERIZE_LOOP;
    }

    void Framebuffer::RenderTriangle(
        glm::vec4 a, glm::vec4 b, glm::vec4 c, FragShaderBase& fs, const Barycentric& bary
    ) {

        // y sort
        if (a.y > b.y) std::swap(a, b);
        if (b.y > c.y) std::swap(b, c);
        if (a.y > b.y) std::swap(a, b);

        // flat bottom
        if (u32(b.y) == u32(c.y)) {
            if (b.x > c.x) std::swap(b, c);
            this->RenderTop(a, b, c, fs, bary);
        }

        // flat top
        else if (u32(a.y) == u32(b.y)) {
            if (a.x > b.x) std::swap(a, b);
            this->RenderBottom(a, b, c, fs, bary);
        }

        else {
            f32       amount = (b.y - a.y) / (c.y - a.y);
            glm::vec4 d      = a + amount * (c - a);

            if (b.x > d.x) std::swap(b, d);
            this->RenderTop(a, b, d, fs, bary);
            this->RenderBottom(b, d, c, fs, bary);
        }
    }

} // namespace mt

namespace mt {

    void Framebuffer::RenderElements(ElementBuffer& ebo, VertexShaderBase& vs, FragShaderBase& fs) {

        u32  elements    = ebo.vbo.GetPerVertex();
        f32* transformed = ebo.vbo.GetTransformed().data();

        std::memmove(transformed, ebo.vbo.GetData(), ebo.vbo.GetCount() * sizeof(f32));
        vs.attribs = transformed;

        for (vs.offset = 0, vs.id = 0; vs.id < ebo.vbo.GetCount() / elements;
             ++vs.id, vs.offset += elements) {

            vs();
            auto& vp = *(glm::vec4*)(vs.attribs + vs.offset);

            vp.w     = 1.f / vp.w;
            vp.x *= vp.w;
            vp.y *= vp.w;
            vp.z *= vp.w;

            vp.x = (1.f + vp.x) * 0.5f * m_width;
            vp.y = (1.f - vp.y) * 0.5f * m_height;
        }

        u64 i = 0;
        for (fs.id = 0; i < ebo.ibo.GetCount(); i += 3, ++fs.id) {
            fs.attribs[0] = transformed + ebo.ibo[i + 0] * elements;
            fs.attribs[1] = transformed + ebo.ibo[i + 1] * elements;
            fs.attribs[2] = transformed + ebo.ibo[i + 2] * elements;

            auto&       a = *(glm::vec4*)(fs.attribs[0]);
            auto&       b = *(glm::vec4*)(fs.attribs[1]);
            auto&       c = *(glm::vec4*)(fs.attribs[2]);

            Barycentric bary(a, b, c, cull_backfaces);
            if (!bary.is_valid) continue;
            if (cull_backfaces && bary.is_backface) continue;

            if (!wireframe) {
                this->RenderTriangle(a, b, c, fs, bary);
            } else {
                this->RenderLine(a, b, ~0u);
                this->RenderLine(b, c, ~0u);
                this->RenderLine(c, a, ~0u);
            }
        }
    }

} // namespace mt

// line

#include <algorithm>

namespace mt {

    void Framebuffer::RenderLine(const glm::vec4& v0, const glm::vec4& v1, u32 color) {

        f32 dx = v1.x - v0.x;
        if (u32(dx) == 0) {
            auto [start, end] = std::minmax({ v0.y, v1.y });
            for (u32 y = u32(start); y <= u32(end); ++y) m_color[y * m_width + u32(v0.x)] = color;
            return;
        }

        f32 dy = v1.y - v0.y;
        if (u32(dy) == 0) {
            auto [start, end] = std::minmax({ v0.x, v1.x });
            for (u32 x = u32(start); x <= u32(end); ++x) m_color[u32(v0.y) * m_width + x] = color;
            return;
        }

        f32 slope = dy / dx;
        f32 y     = v0.y;

        for (u32 x = u32(v0.x); x < u32(v1.x); ++x, y += slope)
            m_color[u32(y) * m_width + x] = color;
    }

} // namespace mt

#endif
