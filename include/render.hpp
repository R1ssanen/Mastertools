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

namespace mt {

    template <typename F>
    void Framebuffer::RenderTop(
        const glm::vec4& v0, const glm::vec4& v1, const glm::vec4& v2, F&& shader, u64 id,
        const Barycentric& bary
    ) {
        glm::vec4 a = v0, b = v1, c = v2;
        if (b.x > c.x) std::swap(b, c);

        f32       slope_x0 = (b.x - a.x) / (b.y - a.y);
        f32       slope_x1 = (c.x - a.x) / (c.y - a.y);

        f32       slope_w0 = (c.w - b.w) / (c.x - b.x);
        f32       slope_w1 = (b.w - a.w) / (b.y - a.y);
        f32       w0       = a.w;

        f32       start_x  = a.x;
        f32       end_x    = a.x;

        glm::vec3 dx       = bary.Get(b.x + 1, b.y) - bary.Get(b.x, b.y);
        glm::vec3 bc0      = bary.Get(a.x, a.y);
        glm::vec3 dy       = bary.Get(a.x + slope_x0, a.y + 1) - bc0;

        for (u32 y = u32(a.y); y < u32(c.y); ++y) {
            u64       row = y * m_width;
            f32       w   = w0;
            glm::vec3 bc  = bc0;

            for (u32 x = u32(start_x); x <= u32(end_x); ++x) {

                f32  z     = glm::dot(bc, glm::vec3(a.z, b.z, c.z)) * w;
                f32& depth = m_depth[row + x];
                if (depth > z) continue;

                depth            = z;
                m_color[row + x] = shader(bc, id);
                // m_color[row + x] = u32(glm::packUnorm4x8(glm::vec4(1.f, z, z, z)));

                w += slope_w0;
                bc += dx;
            }

            start_x += slope_x0;
            end_x += slope_x1;
            w0 += slope_w1;
            bc0 += dy;
        }
    }

    template <typename F>
    void Framebuffer::RenderBottom(
        const glm::vec4& v0, const glm::vec4& v1, const glm::vec4& v2, F&& shader, u64 id,
        const Barycentric& bary
    ) {
        glm::vec4 a = v0, b = v1, c = v2;
        if (a.x > b.x) std::swap(a, b);

        f32       slope_x0 = (c.x - a.x) / (c.y - a.y);
        f32       slope_x1 = (c.x - b.x) / (c.y - b.y);

        f32       slope_w0 = (b.w - a.w) / (b.x - a.x);
        f32       slope_w1 = (c.w - a.w) / (c.y - a.y);
        f32       w0       = a.w;

        f32       start_x  = a.x;
        f32       end_x    = b.x;

        glm::vec3 bc0      = bary.Get(a.x, a.y);
        glm::vec3 dx       = bary.Get(a.x + 1, a.y) - bc0;
        glm::vec3 dy       = bary.Get(a.x + slope_x0, a.y + 1) - bc0;

        for (u32 y = u32(a.y); y < u32(c.y); ++y) {
            u64       row = y * m_width;
            f32       w   = w0;
            glm::vec3 bc  = bc0;

            for (u32 x = u32(start_x); x <= u32(end_x); ++x) {

                f32  z     = glm::dot(bc, glm::vec3(a.z, b.z, c.z)) * w;
                f32& depth = m_depth[row + x];
                if (depth > z) continue;

                depth            = z;
                m_color[row + x] = shader(bc, id);
                // m_color[row + x] = u32(glm::packUnorm4x8(glm::vec4(1.f, z, z, z)));

                w += slope_w0;
                bc += dx;
            }

            start_x += slope_x0;
            end_x += slope_x1;
            w0 += slope_w1;
            bc0 += dy;
        }
    }

    template <typename F>
    void Framebuffer::RenderTriangle(
        const glm::vec4& v0, const glm::vec4& v1, const glm::vec4& v2, F&& shader, u64 id,
        const Barycentric& bary
    ) {
        glm::vec4 a = v0, b = v1, c = v2;

        // y sort
        if (a.y > b.y) std::swap(a, b);
        if (b.y > c.y) std::swap(b, c);
        if (a.y > b.y) std::swap(a, b);

        if (int(b.y) == int(c.y)) {
            this->RenderTop(a, b, c, std::forward<F>(shader), id, bary);
            return;
        }

        if (int(a.y) == int(b.y)) {
            this->RenderBottom(a, b, c, std::forward<F>(shader), id, bary);
            return;
        }

        f32       scalar = (b.y - a.y) / (c.y - a.y);
        glm::vec4 slopes = (c - a) * scalar;
        glm::vec4 d      = a + slopes;

        this->RenderTop(a, b, d, std::forward<F>(shader), id, bary);
        this->RenderBottom(b, d, c, std::forward<F>(shader), id, bary);
    }

} // namespace mt

namespace mt {

    template <typename V, typename F>
    void
    Framebuffer::RenderElements(const ElementBuffer& ebo, V&& vertex_shader, F&& fragment_shader) {

        u64 total_vertices = ebo.vbo.GetCount() / ebo.vbo.GetPerVertex();
        std::memset(ebo.positions, 0, ebo.vbo.GetCount() * sizeof(f32));
        u8* byte_data = (u8*)ebo.vbo.GetData();

        for (u64 i = 0; i < total_vertices; ++i) {

            u64 offset             = i * ebo.vbo.GetVertexStride();
            ebo.positions[i]       = vertex_shader((void*)(byte_data + offset));

            glm::vec4& MT_POSITION = ebo.positions[i];
            MT_POSITION.w          = 1.f / MT_POSITION.w;
            MT_POSITION.x *= MT_POSITION.w;
            MT_POSITION.y *= MT_POSITION.w;
            MT_POSITION.z *= MT_POSITION.w;

            MT_POSITION.x = (MT_POSITION.x + 1.f) * 0.5f * m_width;
            MT_POSITION.y = (MT_POSITION.y + 1.f) * 0.5f * m_height;
        }

        u64 face_id = 0;
        for (u64 j = 0; j < ebo.ibo.GetCount(); j += 3, ++face_id) {
            const glm::vec4& a    = ebo.positions[ebo.ibo[j]];
            const glm::vec4& b    = ebo.positions[ebo.ibo[j + 1]];
            const glm::vec4& c    = ebo.positions[ebo.ibo[j + 2]];

            auto             bary = Barycentric(a, b, c, cull_backfaces);
            if (cull_backfaces && bary.IsBackface()) continue;

            if (!wireframe) {
                this->RenderTriangle(a, b, c, std::forward<F>(fragment_shader), face_id, bary);
            } else {
                this->RenderLine(a, b, ~0u);
                this->RenderLine(b, c, ~0u);
                this->RenderLine(c, a, ~0u);
            }
        }
    }

} // namespace mt

// barycentric

#if 0
namespace mt {

    template <typename F>
    void RenderTriangleB(
        const Vertex& a, const Vertex& b, const Vertex& c, Framebuffer&& framebuffer, F&& shader,
        u64 id, const Barycentric& bary
    ) {
        auto [minx, maxx] = std::minmax({ a.pos.x, b.pos.x, c.pos.x });
        auto [miny, maxy] = std::minmax({ a.pos.y, b.pos.y, c.pos.y });

        f32  A01 = a.pos.y - b.pos.y, B01 = b.pos.x - a.pos.x;
        f32  A12 = b.pos.y - c.pos.y, B12 = c.pos.x - b.pos.x;
        f32  A20 = c.pos.y - a.pos.y, B20 = a.pos.x - c.pos.x;

        auto determinant = [](const auto& a, const auto& b, f32 minx, f32 miny) {
            return (b.x - a.x) * (miny - a.y) - (b.y - a.y) * (minx - a.x);
        };

        f32 w0_row = determinant(b.pos, c.pos, minx, miny);
        f32 w1_row = determinant(c.pos, a.pos, minx, miny);
        f32 w2_row = determinant(a.pos, b.pos, minx, miny);

        for (f32 y = std::max(miny, 0.f); y <= std::min(maxy, 899.f); ++y) {
            f32 w0 = w0_row;
            f32 w1 = w1_row;
            f32 w2 = w2_row;

            for (f32 x = std::max(minx, 0.f); x <= std::min(maxx, 1440.f); ++x) {
                glm::vec3 bc = bary.Get(x, y);
                // std::clog << std::format("{}, {}, {}\n", bc.x, bc.y, bc.z);

                if (bc.x >= 0 && bc.y >= 0 && bc.z >= 0)
                    framebuffer[u32(y) * framebuffer.width + u32(x)] = shader(bc, id);

                w0 += A12;
                w1 += A20;
                w2 += A01;
            }

            w0_row += B12;
            w1_row += B20;
            w2_row += B01;
        }
    }

} // namespace mt
#endif
// line

#include <algorithm>

namespace mt {

    void Framebuffer::RenderLine(const glm::vec4& v0, const glm::vec4& v1, u32 color) {

        f32 dx = v1.x - v0.x;
        if (u32(dx) == 0) {
            auto [start, end] = std::minmax(u32(v0.y), u32(v1.y));
            for (u32 y = start; y <= end; ++y) m_color[y * m_width + u32(v0.x)] = color;
            return;
        }

        f32 dy = v1.y - v0.y;
        if (u32(dy) == 0) {
            auto [start, end] = std::minmax(u32(v0.x), u32(v1.x));
            for (u32 x = start; x <= end; ++x) m_color[u32(v0.y) * m_width + x] = color;
            return;
        }

        f32 slope = dy / dx;
        f32 y     = v0.y;

        for (u32 x = u32(v0.x); x <= u32(v1.x); ++x, y += slope)
            m_color[u32(y) * m_width + x] = color;
    }

} // namespace mt

#endif
