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

//
#include "edge.hpp"

namespace mt {

    constexpr u32 wireframe_color = 0x00dca07f;

    void          Framebuffer::render_triangle(
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
        auto [p0, p1, p2] = [&uo_p0, &uo_p1, &uo_p2] {
            auto* p0 = &uo_p0;
            auto* p1 = &uo_p1;
            auto* p2 = &uo_p2;

            if (p0->y > p1->y) std::swap(p0, p1);
            if (p1->y > p2->y) std::swap(p1, p2);
            if (p0->y > p1->y) std::swap(p0, p1);

            return std::tie(*p0, *p1, *p2);
        }();

        glm::vec3 z_pack    = { uo_p0.z, uo_p1.z, uo_p2.z };
        glm::vec3 w_pack    = { uo_p0.w, uo_p1.w, uo_p2.w };

        auto      rasterize = [this, &frag, &bary, &z_pack, &w_pack](
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
                    frag.loc   = row + u32(x);

                    frag.pos.z = z;
                    f32& depth = m_depth[frag.loc];

                    if (depth > frag.pos.z) {
                        depth          = frag.pos.z;

                        glm::vec3 term = b * w_pack;
                        frag.barycoord = term / (term.x + term.y + term.z);

                        frag.pos.x     = x;
                        frag.pos.y     = y;
                        frag.w         = w;

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

        u32 per_vertex = vbo.GetPerVertex();
        vertex.attribs = vbo.GetData();

        std::vector<glm::vec4> vertices;
        vertices.reserve(vbo.GetCount());

        for (vertex.offset = 0, vertex.id = 0; vertex.id < vbo.GetCount() / per_vertex;
             ++vertex.id, vertex.offset += per_vertex) {
            vertices.push_back(vertex());
        }

#if 1
        auto hash_one_u32 = [](u32 x) noexcept -> u32 {
            x += (x << 10);
            x ^= (x >> 6);
            x += (x << 3);
            x ^= (x >> 11);
            x += (x << 15);
            return x;
        };

        for (u64 id = 1, i = 0; i < ibo.GetCount(); i += 3, ++id) {
            frag.id = hash_one_u32(id);
            u32 id0 = ibo[i], id1 = ibo[i + 1], id2 = ibo[i + 2];

            frag.attribs[0] = vbo.GetData() + per_vertex * id0;
            frag.attribs[1] = vbo.GetData() + per_vertex * id1;
            frag.attribs[2] = vbo.GetData() + per_vertex * id2;

            auto p0 = vertices[id0], p1 = vertices[id1], p2 = vertices[id2];

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
                    this->render_line(p0.x, p0.y, p1.x, p1.y, wireframe_color);
                    this->render_line(p1.x, p1.y, p2.x, p2.y, wireframe_color);
                    this->render_line(p2.x, p2.y, p0.x, p0.y, wireframe_color);
                }
            };

            for (auto& [p0, p1, p2] : clip_triangle_homo_frustum(
                     p0, p1, p2, frag.attribs[0], frag.attribs[1], frag.attribs[2], m_clip_frustum
                 )) {

                postclip_transform(p0);
                postclip_transform(p1);
                postclip_transform(p2);
                render_triangle(p0, p1, p2);
            }
        }

#else

        std::unordered_map<u32, Edge> edge_table;
        std::vector<u32>              new_indices;
        /*std::vector<u32>              new_indices =
            [edges = form_edges(ibo.GetData(), ibo.GetCount(), DrawMode::TRIANGLE)] {
                auto* data    = const_cast<Edge*>(edges.data());
                u32*  indices = reinterpret_cast<u32*>(data);
                return std::vector<u32>(indices, indices + edges.size() * 2);
            }();
        */

        for (auto& edge : form_edges(ibo.GetData(), ibo.GetCount(), DrawMode::TRIANGLE)) {
            auto it = edge_table.find(edge.hash);

            // edge exists
            if (it != edge_table.end()) {
                Edge& existing = it->second;
                new_indices.push_back(existing.id0);
                new_indices.push_back(existing.id1);

            } else {
                bool outside = !clip_edge(vertices[edge.id0], vertices[edge.id1], ClipPlaneNear());
                if (outside) continue;

                edge_table[edge.hash] = edge;
                new_indices.push_back(edge.id0);
                new_indices.push_back(edge.id1);
            }
        }

        u32 triangle_id = 0xff0000ff;
        for (u64 i = 0; i < new_indices.size(); i += 2) {
            frag.id                 = triangle_id;

            auto postclip_transform = [this](glm::vec4& p) {
                p.w = 1.f / p.w;
                p.x *= p.w;
                p.y *= p.w;
                p.z *= p.w;

                // std::clog << p.x << ' ' << p.y << ' ' << p.z << '\n';

                p.x = std::clamp(0.5f + p.x * 0.5f, 0.f, 1.f);
                p.y = std::clamp(0.5f - p.y * 0.5f, 0.f, 1.f);

                p.x = std::trunc(p.x * m_width - 0.5f);
                p.y = std::trunc(p.y * m_height - 0.5f);
            };

            glm::vec4 p0 = vertices[new_indices[i]];
            glm::vec4 p1 = vertices[new_indices[i + 1]];
            // glm::vec4 p2 = vertices[new_indices[i + 2]];

            postclip_transform(p0);
            postclip_transform(p1);
            // postclip_transform(p2);

            render_line(p0.x, p0.y, p1.x, p1.y, 0xff0000ff);

            /*Barycentric bary(p0, p1, p2, cull_backfaces);

            if (!bary.is_valid) return;
            if (cull_backfaces && bary.is_backface) return;

            this->render_triangle(p0, p1, p2, frag, bary);

            if (wireframe) {
                this->render_line(p0.x, p0.y, p1.x, p1.y, ~0);
                this->render_line(p1.x, p1.y, p2.x, p2.y, ~0);
                this->render_line(p2.x, p2.y, p0.x, p0.y, ~0);
            }*/
        }

#endif
    }

} // namespace mt

namespace mt {

    void Framebuffer::render_line(u32 x0, u32 y0, u32 x1, u32 y1, u32 color) {

        i32 dx = x1 - x0;
        if (dx == 0) { // vertical
            auto [miny, maxy] = std::minmax(y0, y1);
            u64 index         = miny * m_width + x0;

            for (u64 y = miny; y < maxy; ++y) {
                m_color[index] = color;
                index += m_width;
            }
            return;
        }

        i32 dy = y1 - y0;
        if (dy == 0) { // horizontal
            u64 begin = y0 * m_width + std::min(x0, x1);
            std::fill_n(&m_color[begin], std::abs(dx), color);
            return;
        }

        if (std::abs(dx) > std::abs(dy)) { // gentle slope < 45deg
            if (x0 > x1) {
                std::swap(x0, x1);
                y0 = y1;
            }

            f32 slope = f32(dy) / f32(dx);
            f32 y     = y0;

            for (u32 x = x0; x < x1; ++x, y += slope) m_color[u32(y) * m_width + x] = color;
        }

        else { // steep slope >= 45deg
            if (y0 > y1) {
                std::swap(y0, y1);
                x0 = x1;
            }

            f32 slope = f32(dx) / f32(dy);
            u64 row   = y0 * m_width;
            f32 x     = x0;

            for (u32 y = y0; y < y1; ++y, x += slope) {
                m_color[row + u32(x)] = color;
                row += m_width;
            }
        }
    }

} // namespace mt

#if 0
namespace mt {

    void Framebuffer::render_triangle_edge(
        const glm::vec4& p0, const glm::vec4& p1, const glm::vec4& p2, FragShaderBase& frag,
        const Barycentric& bary
    ) {

        auto [minx_uc, maxx_uc] = std::minmax({ p0.x, p1.x, p2.x });
        u32 minx                = std::clamp(u64(minx_uc), 0ull, m_width);
        u32 maxx                = std::clamp(u64(maxx_uc), 0ull, m_width);
        if (minx == maxx) return;

        auto [miny_uc, maxy_uc] = std::minmax({ p0.y, p1.y, p2.y });
        u32 miny                = std::clamp(u64(miny_uc), 0ull, m_height);
        u32 maxy                = std::clamp(u64(maxy_uc), 0ull, m_height);
        if (miny == maxy) return;

        glm::vec3 slope_b0 = bary.GetDeltaX();
        glm::vec3 slope_b1 = bary.GetDeltaYNoSlope();
        glm::vec3 b0       = bary.GetCoord(minx, miny);

        glm::vec3 z_pack   = { p0.z, p1.z, p2.z };
        u64       row      = miny * m_width;

        if (u32(p0.y) == u32(p1.y) || u32(p1.y) == u32(p2.y) || u32(p2.y) == u32(p0.y)) goto level;

        for (u32 y = miny; y <= maxy; ++y, row += m_width) {
            frag.barycoord = b0;
            bool inside    = false;

            for (u32 x = minx; x <= maxx; ++x, frag.barycoord += slope_b0) {

                auto eq_zero = glm::equal(frag.barycoord, glm::zero<glm::vec3>());
                if (glm::any(eq_zero)) {
                    if (inside) break;

                    inside = true;
                    goto inside;
                }

                if (glm::all(glm::greaterThan(frag.barycoord, glm::zero<glm::vec3>()))) {
                inside:
                    m_color[row + x] = 0xff0000ff;
                }

                /*if (glm::any(glm::lessThan(frag.barycoord, glm::zero<glm::vec3>()))) continue;

                inside = true;
                frag.pos.z = glm::dot(z_pack, frag.barycoord);
                f32& depth = m_depth[row + x];
                if (depth < frag.pos.z) continue;

                m_color[row + x] = 0;
                // frag(m_color[row + x]);
                */
            }

            b0 += slope_b1;
        }

    level:
    }
} // namespace mt
#endif

#endif
