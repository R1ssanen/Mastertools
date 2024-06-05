#pragma once

#include "../core/buffer.hpp"
#include "../core/triangle.hpp"
#include "../core/vertex.hpp"
#include "../mtpch.hpp"
#include "../resource/shader.hpp"
#include "../resource/texture.hpp"
#include "context.hpp"

namespace mt {

    void       DrawWireframe(Context& Context, const Triangle& Tri, u8 r, u8 g, u8 b, u8 a = 100);

    inline f32 Edge(f32 ax, f32 ay, f32 bx, f32 by, f32 cx, f32 cy) {
        return (cx - ax) * (by - ay) - (cy - ay) * (bx - ax);
    }

    inline f32 DoubleTriangleArea(f32 ax, f32 ay, f32 bx, f32 by, f32 cx, f32 cy) {
        return glm::dot(glm::vec3(ax, bx, cx), glm::vec3(by - cy, cy - ay, ay - by));
    }

    void RenderTriBary(buffer_t Buffer, Triangle& Tri, f32 InverseFar);

    void RenderTriScan(buffer_t Buffer, Triangle& Tri, f32 InverseFar);

    /*
    void RenderTriScan(Context& Context, triangle_t& Tri, texture_t Texture, f32 InverseFar,
                       const shader_t& Shader);
    */
} // namespace mt
