#pragma once

#include <initializer_list>

#include "context.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "vertex.hpp"
#include "buffer.hpp"

namespace core
{

void DrawWireframe(Context& t_Context, const triangle_t& t_Tri, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 100);

inline float Edge(float ax, float ay, float bx, float by, float cx, float cy)
{
    return (cx - ax) * (by - ay) - (cy - ay) * (bx - ax);
}

inline float DoubleTriangleArea(float ax, float ay, float bx, float by, float cx, float cy)
{
    return ax * (by - cy) + bx * (cy - ay) + cx * (ay - by);
}

inline glm::vec3 CalculateMatrixRow(const glm::vec3& A, const glm::vec3& B)
{
    return glm::vec3(A.x * B.y - B.x * A.y, A.y - B.y, B.x - A.x);
}

void RenderTriBary(buffer_t t_Buffer, DrawTri t_DrawTri, float t_InverseFar);

/*
void RenderTriScan(Context& t_Context, triangle_t& t_Tri, texture_t t_Texture, float t_InverseFar,
                   const shader_t& t_Shader);
*/
} // namespace core
