#ifndef MT_RENDER_HPP_
#define MT_RENDER_HPP_

#include <algorithm>
#include <cstring>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <iostream>
#include <utility>

#include "barycentric.hpp"
#include "buffers/buffer.hpp"
#include "buffers/framebuffer.hpp"
#include "buffers/ib.hpp"
#include "clipping.hpp"
#include "mtdefs.hpp"

namespace mt {

    void Framebuffer::RenderTriangle(
        glm::vec4 a, glm::vec4 b, glm::vec4 c, FragShaderBase& frag, const Barycentric& bary
    ) {

        // y sort
        if (a.y > b.y) std::swap(a, b);
        if (b.y > c.y) std::swap(b, c);
        if (a.y > b.y) std::swap(a, b);

        auto rasterize = [this, &frag, &bary](
                             f32 x0, f32 x1, f32 y0, f32 y1, f32 w0, f32 slope_xl, f32 slope_xr,
                             f32 slope_w0, f32 slope_w1
                         ) {
            glm::vec3 slope_b0 = bary.GetDeltaX();
            glm::vec3 slope_b1 = bary.GetDeltaY(slope_xl);
            glm::vec3 b0       = bary.GetCoord(x0, y0);

            u64       row      = u64(y0) * m_width;
            for (u32 y = u32(y0), i = 0; y < u32(y1); ++y, ++i) {

                f32       w = w0;
                glm::vec3 b = b0;

                for (u32 x = u32(x0 + slope_xl * i); x < u32(x1 + slope_xr * i); ++x) {

                    if (m_depth[row + x] > w) continue;
                    else
                        m_depth[row + x] = w;

                    frag.barycoord = b * w;
                    frag.loc       = row + x;
                    frag.pos       = glm::vec2(x, y);
                    frag(m_color[frag.loc]);

                    w += slope_w0;
                    b += slope_b0;
                }

                w0 += slope_w1;
                b0 += slope_b1;
                row += m_width;
            }
        };

        // flat top
        if (u32(a.y) == u32(b.y)) {
            if (a.x > b.x) std::swap(a, b);

            f32 slope_xl = (c.x - a.x) / (c.y - a.y);
            f32 slope_xr = (c.x - b.x) / (c.y - b.y);
            f32 slope_w0 = (b.w - a.w) / (b.x - a.x);
            f32 slope_w1 = (c.w - a.w) / (c.y - a.y);

            rasterize(a.x, b.x, a.y, c.y, a.w, slope_xl, slope_xr, slope_w0, slope_w1);
        }

        // flat bottom
        else if (u32(b.y) == u32(c.y)) {
            if (b.x > c.x) std::swap(b, c);

            f32 slope_xl = (b.x - a.x) / (b.y - a.y);
            f32 slope_xr = (c.x - a.x) / (c.y - a.y);
            f32 slope_w0 = (c.w - b.w) / (c.x - b.x);
            f32 slope_w1 = (b.w - a.w) / (b.y - a.y);

            rasterize(a.x, a.x, a.y, b.y, a.w, slope_xl, slope_xr, slope_w0, slope_w1);
        }

        else {
            f32       amount       = (b.y - a.y) / (c.y - a.y);
            glm::vec4 d            = a + amount * (c - a);

            f32       slope_shared = (c.x - a.x) / (c.y - a.y);
            f32       slope_xt     = (b.x - a.x) / (b.y - a.y);
            f32       slope_xb     = (c.x - b.x) / (c.y - b.y);

            if (b.x > d.x) {
                f32 slope_w0 = (b.w - d.w) / (b.x - d.x);
                f32 slope_w1 = (c.w - a.w) / (c.y - a.y);

                rasterize(a.x, a.x, a.y, b.y, a.w, slope_shared, slope_xt, slope_w0, slope_w1);
                rasterize(d.x, b.x, d.y, c.y, d.w, slope_shared, slope_xb, slope_w0, slope_w1);

            } else {
                f32 slope_w0  = (d.w - b.w) / (d.x - b.x);
                f32 slope_w10 = (b.w - a.w) / (b.y - a.y);
                f32 slope_w11 = (c.w - b.w) / (c.y - b.y);

                rasterize(a.x, a.x, a.y, b.y, a.w, slope_xt, slope_shared, slope_w0, slope_w10);
                rasterize(b.x, d.x, b.y, c.y, b.w, slope_xb, slope_shared, slope_w0, slope_w11);
            }
        }
    }

} // namespace mt

namespace mt {

    void Framebuffer::RenderElements(
        ElementBuffer& ebo, VertexShaderBase& vertex, FragShaderBase& frag
    ) {

        u32  elements    = ebo.vbo.GetPerVertex();
        f32* transformed = ebo.vbo.GetTransformed().data();

        std::memmove(transformed, ebo.vbo.GetData(), ebo.vbo.GetCount() * sizeof(f32));
        vertex.attribs = transformed;

        for (vertex.offset = 0, vertex.id = 0; vertex.id < ebo.vbo.GetCount() / elements;
             ++vertex.id, vertex.offset += elements) {
            vertex();
        }

        auto hash = [](u32 x) noexcept -> u32 {
            x += (x << 10);
            x ^= (x >> 6);
            x += (x << 3);
            x ^= (x >> 11);
            x += (x << 15);
            return x;
        };

        for (u64 id = 0, i = 0; i < ebo.ibo.GetCount(); i += 3, ++id) {
            frag.id                 = hash(id);

            frag.attribs[0]         = transformed + ebo.ibo[i + 0] * elements;
            frag.attribs[1]         = transformed + ebo.ibo[i + 1] * elements;
            frag.attribs[2]         = transformed + ebo.ibo[i + 2] * elements;

            auto a                  = *(glm::vec4*)(frag.attribs[0]);
            auto b                  = *(glm::vec4*)(frag.attribs[1]);
            auto c                  = *(glm::vec4*)(frag.attribs[2]);

            auto postclip_transform = [this](glm::vec4& p) {
                p.w = 1.f / p.w;
                p.x *= p.w;
                p.y *= p.w;
                p.z *= p.w;

                p.x = std::clamp(0.5f + p.x * 0.5f, 0.f, 1.f);
                p.y = std::clamp(0.5f - p.y * 0.5f, 0.f, 1.f);

                p.x = std::trunc(p.x * m_width - 0.5f);
                p.y = std::trunc(p.y * m_height - 0.5f);
            };

            auto render_triangle = [this, &frag](const auto& a, const auto& b, const auto& c) {
                Barycentric bary(a, b, c, cull_backfaces);

                if (!bary.is_valid) return;
                if (cull_backfaces && bary.is_backface) return;

                this->RenderTriangle(a, b, c, frag, bary);

                if (wireframe) {
                    this->RenderLine(a.x, a.y, b.x, b.y, ~0);
                    this->RenderLine(b.x, b.y, c.x, c.y, ~0);
                    this->RenderLine(c.x, c.y, a.x, a.y, ~0);
                }
            };

            for (auto& [p0, p1, p2] : clip_triangle_homo_frustum(a, b, c, m_clip_frustum)) {
                postclip_transform(p0);
                postclip_transform(p1);
                postclip_transform(p2);
                render_triangle(p0, p1, p2);
            }
        }
    }

} // namespace mt

namespace mt {

    void Framebuffer::RenderLine(u32 x0, u32 y0, u32 x1, u32 y1, u32 color) {

        int dx = x1 - x0;
        if (dx == 0) {
            for (u32 y = std::min(y0, y1); y <= std::max(y0, y1); ++y)
                m_color[y * m_width + x0] = color;
            return;
        }

        int dy = y1 - y0;
        if (dy == 0) {
            u64 begin = y0 * m_width + std::min(x0, x1);
            std::fill_n(&m_color[begin], std::abs(dx), color);
            return;
        }

        // under 45 degrees
        if (std::abs(dx) > std::abs(dy)) {
            if (x0 > x1) {
                std::swap(x0, x1);
                y0 = y1;
            }

            f32 slope = f32(dy) / f32(dx);
            f32 y     = y0;
            for (u32 x = x0; x <= x1; ++x, y += slope) m_color[u32(y) * m_width + x] = color;
        }

        // over or even 45 degrees
        else {
            if (y0 > y1) {
                std::swap(y0, y1);
                x0 = x1;
            }

            f32 slope = f32(dx) / f32(dy);
            f32 x     = x0;
            for (u32 y = y0; y <= y1; ++y, x += slope) m_color[y * m_width + u32(x)] = color;
        }
    }

} // namespace mt

#endif
