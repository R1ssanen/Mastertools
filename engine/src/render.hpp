#pragma once

#include "context.hpp"
#include "vertex.hpp"

namespace core {

void DrawLine(Context& t_Context,
              const glm::vec3& a,
              const glm::vec3& b,
              uint32_t col = 0xFFffdd46);

float Edge(float ax, float ay, float bx, float by, float cx, float cy);

void RenderTri(Context& t_Context,
               const Tri& t_Tri,
               const std::string& t_TexName);

}  // namespace core
