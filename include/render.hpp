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
#include "shader.hpp"

namespace mt {

    void Framebuffer::render_triangle(
        const glm::vec4& uo_p0, const glm::vec4& uo_p1, const glm::vec4& uo_p2,
        FragShaderBase& frag, const Barycentric& bary
    ) {

        auto get_flat_top_slopes = [](const auto& p0, const auto& p1, const auto& p2) {
            f32 inv_p0p2_y = 1.f / (p2.y - p0.y);
            f32 inv_p0p1_x = 1.f / (p1.x - p0.x);

            f32 slope_xl   = (p2.x - p0.x) * inv_p0p2_y;
            f32 slope_xr   = (p2.x - p1.x) / (p2.y - p1.y);

            f32 slope_z0   = (p1.z - p0.z) * inv_p0p1_x;
            f32 slope_z1   = (p2.z - p0.z) * inv_p0p2_y;

            f32 slope_w0   = (p1.w - p0.w) * inv_p0p1_x;
            f32 slope_w1   = (p2.w - p0.w) * inv_p0p2_y;

            return std::array{ p0.x,     p1.x,     p0.y,     p2.y,     p0.z,     p0.w,
                               slope_xl, slope_xr, slope_z0, slope_z1, slope_w0, slope_w1 };
        };

        auto get_flat_bottom_slopes = [](const auto& p0, const auto& p1, const auto& p2) {
            f32 inv_p0p2_y = 1.f / (p2.y - p0.y);
            f32 inv_p1p2_x = 1.f / (p2.x - p1.x);

            f32 slope_xl   = (p1.x - p0.x) * inv_p0p2_y;
            f32 slope_xr   = (p2.x - p0.x) * inv_p0p2_y;

            f32 slope_z0   = (p2.z - p1.z) * inv_p1p2_x;
            f32 slope_z1   = (p1.z - p0.z) * inv_p0p2_y;

            f32 slope_w0   = (p2.w - p1.w) * inv_p1p2_x;
            f32 slope_w1   = (p1.w - p0.w) * inv_p0p2_y;

            return std::array{ p0.x,     p0.x,     p0.y,     p1.y,     p0.z,     p0.w,
                               slope_xl, slope_xr, slope_z0, slope_z1, slope_w0, slope_w1 };
        };

        auto vec_x_less   = [](auto& lhs, auto& rhs) { return lhs.x < rhs.x; };

        // y-ordered
        auto [p0, p1, p2] = [&uo_p0, &uo_p1, &uo_p2]() {
            auto* p0 = &uo_p0;
            auto* p1 = &uo_p1;
            auto* p2 = &uo_p2;

            if (p0->y > p1->y) std::swap(p0, p1);
            if (p1->y > p2->y) std::swap(p1, p2);
            if (p0->y > p1->y) std::swap(p0, p1);

            return std::tie(*p0, *p1, *p2);
        }();

        glm::vec3 z_pack    = { uo_p0.z, uo_p1.z, uo_p2.z };

        auto      rasterize = [this, &frag, &bary, &z_pack](
                             f32 x0, f32 x1, f32 y0, f32 y1, f32 z0, f32 w0, f32 slope_xl,
                             f32 slope_xr, f32 slope_z0, f32 slope_z1, f32 slope_w0, f32 slope_w1
                         ) {
            glm::vec3 slope_b0 = bary.GetDeltaX();
            glm::vec3 slope_b1 = bary.GetDeltaY(slope_xl);
            glm::vec3 b0       = bary.GetCoord(x0, y0);

            u64       row      = u64(y0) * m_width;
            u32       y_step   = 0;

            for (f32 y = std::trunc(y0); y < std::trunc(y1); ++y, ++y_step, row += m_width) {

                glm::vec3 b = b0;
                f32       w = w0;
                f32       z = z0;

                for (f32 x = std::trunc(slope_xl * y_step + x0);
                     x < std::trunc(slope_xr * y_step + x1); ++x) {

                    glm::vec3 term = b / w;
                    frag.barycoord = term / (term.x + term.y + term.z);
                    frag.pos.z     = glm::dot(frag.barycoord, z_pack);

                    frag.loc       = row + u32(x);
                    // frag.pos.z = z / w;

                    f32& depth     = m_depth[frag.loc];

                    if (depth > frag.pos.z) {
                        depth      = frag.pos.z;
                        frag.pos.x = x;
                        frag.pos.y = y;
                        frag(m_color[frag.loc]);
                    }

                    b += slope_b0;
                    w += slope_w0;
                    z += slope_z0;
                }

                b0 += slope_b1;
                w0 += slope_w1;
                z0 += slope_z1;
            }
        };

        // flat top
        if (u32(p0.y) == u32(p1.y)) {
            auto [left, right] = std::minmax(p0, p1, vec_x_less);
            std::apply(rasterize, get_flat_top_slopes(left, right, p2));
        }

        // flat bottom
        else if (u32(p1.y) == u32(p2.y)) {
            auto [left, right] = std::minmax(p1, p2, vec_x_less);
            std::apply(rasterize, get_flat_bottom_slopes(p0, left, right));
        }

        // general
        else {
            f32       amount   = (p1.y - p0.y) / (p2.y - p0.y);
            glm::vec4 p3       = p0 + amount * (p2 - p0);

            auto [left, right] = std::minmax(p1, p3, vec_x_less);
            std::apply(rasterize, get_flat_bottom_slopes(p0, left, right));
            std::apply(rasterize, get_flat_top_slopes(left, right, p2));
        }
    }

} // namespace mt

namespace mt {

    void Framebuffer::render_elements(
        VertexBuffer& vbo, IndexBuffer& ibo, VertexShaderBase& vertex, FragShaderBase& frag
    ) {

        u32  elements    = vbo.GetPerVertex();
        f32* transformed = vbo.GetTransformed().data();

        std::memmove(transformed, vbo.GetData(), vbo.GetCount() * sizeof(f32));
        vertex.attribs = transformed;

        for (vertex.offset = 0, vertex.id = 0; vertex.id < vbo.GetCount() / elements;
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

        for (u64 id = 0, i = 0; i < ibo.GetCount(); i += 3, ++id) {
            frag.id           = hash(id) | 0x3813daff;

            frag.attribs[0]   = transformed + ibo[i + 0] * elements;
            frag.attribs[1]   = transformed + ibo[i + 1] * elements;
            frag.attribs[2]   = transformed + ibo[i + 2] * elements;

            auto p0           = *(glm::vec4*)(frag.attribs[0]);
            auto p1           = *(glm::vec4*)(frag.attribs[1]);
            auto p2           = *(glm::vec4*)(frag.attribs[2]);

            // NOTE: temporary
            glm::vec3 world_a = clip_to_world(p0, frag.inv_view_proj),
                      world_b = clip_to_world(p1, frag.inv_view_proj),
                      world_c = clip_to_world(p2, frag.inv_view_proj);

            frag.world_normal = glm::normalize(glm::cross(world_b - world_a, world_c - world_a));

            auto postclip_transform = [this](glm::vec4& p) {
                p.w = 1.f / p.w;
                p.x *= p.w;
                p.y *= p.w;

                p.z *= p.w;
                //  std::clog << p.z << ' ' << p.z * p.w << '\n';

                p.x = std::clamp(0.5f + p.x * 0.5f, 0.f, 1.f);
                p.y = std::clamp(0.5f - p.y * 0.5f, 0.f, 1.f);

                p.x = std::trunc(p.x * m_width - 0.5f);
                p.y = std::trunc(p.y * m_height - 0.5f);
            };

            auto render_triangle = [this, &frag](const auto& p0, const auto& p1, const auto& p2) {
                Barycentric bary(p0, p1, p2, cull_backfaces);

                if (!bary.is_valid) return;
                if (cull_backfaces && bary.is_backface) return;

                this->render_triangle(p0, p1, p2, frag, bary);

                if (wireframe) {
                    this->render_line(p0.x, p0.y, p1.x, p1.y, ~0);
                    this->render_line(p1.x, p1.y, p2.x, p2.y, ~0);
                    this->render_line(p2.x, p2.y, p0.x, p0.y, ~0);
                }
            };

            for (auto& [p0, p1, p2] : clip_triangle_homo_frustum(p0, p1, p2, m_clip_frustum)) {
                postclip_transform(p0);
                postclip_transform(p1);
                postclip_transform(p2);
                render_triangle(p0, p1, p2);
            }
        }
    }

} // namespace mt

namespace mt {

    void Framebuffer::render_line(u32 x0, u32 y0, u32 x1, u32 y1, u32 color) {

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
