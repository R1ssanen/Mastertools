#ifndef MT_RENDER_HPP_
#define MT_RENDER_HPP_

#include <algorithm>
#include <cstring>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <utility>

#include "barycentric.hpp"
#include "buffers/buffer.hpp"
#include "buffers/framebuffer.hpp"
#include "buffers/ib.hpp"
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
                             f32 x0, f32 x1, f32 y0, f32 y1, f32 w0, f32 slope_x0, f32 slope_x1,
                             f32 slope_w0, f32 slope_w1
                         ) {
            glm::vec3 slope_b0 = bary.GetDeltaX();
            glm::vec3 slope_b1 = bary.GetDeltaY(slope_x0);
            glm::vec3 b0       = bary.GetCoord(x0, y0);

            u64       row      = u64(y0) * m_width;
            for (u32 y = u32(y0), i = 0; y < u32(y1); ++y, ++i) {

                f32       w = w0;
                glm::vec3 b = b0;

                for (u32 x = u32(x0 + slope_x0 * i); x < u32(x1 + slope_x1 * i); ++x) {

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
                f32 slope_w0  = (b.w - d.w) / (b.x - d.x);
                f32 slope_w10 = (d.w - a.w) / (d.y - a.y);
                f32 slope_w11 = (c.w - d.w) / (c.y - d.y);

                rasterize(a.x, a.x, a.y, b.y, a.w, slope_shared, slope_xt, slope_w0, slope_w10);
                rasterize(d.x, b.x, d.y, c.y, d.w, slope_shared, slope_xb, slope_w0, slope_w11);

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

            vp.x = std::floor((1.f + vp.x) * 0.5f * m_width);
            vp.y = std::floor((1.f - vp.y) * 0.5f * m_height);
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
            fs.id         = hash(id);

            fs.attribs[0] = transformed + ebo.ibo[i + 0] * elements;
            fs.attribs[1] = transformed + ebo.ibo[i + 1] * elements;
            fs.attribs[2] = transformed + ebo.ibo[i + 2] * elements;

            auto& a       = *(glm::vec4*)(fs.attribs[0]);
            auto& b       = *(glm::vec4*)(fs.attribs[1]);
            auto& c       = *(glm::vec4*)(fs.attribs[2]);

            /*auto  inside  = [](auto& v0, auto& v1, auto& v2) noexcept -> u8 {
                bool in0 = (-v0.w <= v0.x && v0.x <= v0.w) && (-v0.w <= v0.y && v0.y <= v0.w) &&
                           (-v0.w <= v0.z && v0.z <= v0.w);

                bool in1 = (-v1.w <= v1.x && v1.x <= v1.w) && (-v1.w <= v1.y && v1.y <= v1.w) &&
                           (-v1.w <= v1.z && v1.z <= v1.w);

                bool in2 = (-v2.w <= v2.x && v2.x <= v2.w) && (-v2.w <= v2.y && v2.y <= v2.w) &&
                           (-v2.w <= v2.z && v2.z <= v2.w);

                return (in2 << 2) | (in1 << 1) | (in0);
            };

            switch (inside(a, b, c)) {
            case 4: std::clog << "All inside.\n"; break;
            default: std::clog << "Not all inside.\n"; break;
            }*/

            /*for (auto& v : std::array{ a, b, c }) {
                v.w = 1.f / v.w;
                v.x *= v.w;
                v.y *= v.w;
                v.z *= v.w;

                v.x = std::floor((1.f + v.x) * 0.5f * m_width);
                v.y = std::floor((1.f - v.y) * 0.5f * m_height);
            }*/

            Barycentric bary(a, b, c, cull_backfaces);
            if (!bary.is_valid) continue;
            if (cull_backfaces && bary.is_backface) continue;

            if (!wireframe) {
                this->RenderTriangle(a, b, c, fs, bary);
            } else {
                this->RenderLine(a.x, a.y, b.x, b.y, ~0);
                this->RenderLine(b.x, b.y, c.x, c.y, ~0);
                this->RenderLine(c.x, c.y, a.x, a.y, ~0);
            }
        }
    }

} // namespace mt

namespace mt {

    void Framebuffer::RenderLine(u32 x0, u32 y0, u32 x1, u32 y1, u32 color) {

        int dx = x1 - x0;
        if (dx == 0) {
            for (u32 y = std::min(y0, y1); y <= std::max(y0, y1); ++y)
                m_color[y * m_width + x0] = 0xff00ffff;
            return;
        }

        int dy = y1 - y0;
        if (dy == 0) {
            u64 begin = y0 * m_width + std::min(x0, x1);
            std::fill_n(&m_color[begin], std::abs(dx), 0x0000ffff);
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
            for (u32 x = x0; x <= x1; ++x, y += slope) m_color[u32(y) * m_width + x] = 0x00ff00ff;
        }

        // over or even 45 degrees
        else {
            if (y0 > y1) {
                std::swap(y0, y1);
                x0 = x1;
            }

            f32 slope = f32(dx) / f32(dy);
            f32 x     = x0;
            for (u32 y = y0; y <= y1; ++y, x += slope) m_color[y * m_width + u32(x)] = 0xff0000ff;
        }
    }

} // namespace mt

#endif
