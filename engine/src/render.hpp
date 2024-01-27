#pragma once

#include <initializer_list>

#include "context.hpp"
#include "vertex.hpp"

namespace core {

void DrawLine(Context& t_Context,
              const glm::vec3& a,
              const glm::vec3& b,
              uint32_t t_Col = 0xFFffdd46);

void DrawWireframe(Context& t_Context,
                   const std::initializer_list<glm::vec3>& t_Points,
                   bool t_Closed = true,
                   uint32_t t_Col = 0xFFffdd46);

inline float Edge(float ax, float ay, float bx, float by, float cx, float cy) {
  return (cx - ax) * (by - ay) - (cy - ay) * (bx - ax);
}

void RenderTri(Context& t_Context,
               const Tri& t_Tri,
               const std::string& t_TexName,
               float t_InverseFar);

}  // namespace core
