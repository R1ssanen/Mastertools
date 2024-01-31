#include "render.hpp"

#include <initializer_list>

#include "context.hpp"
#include "srpch.hpp"
#include "texture.hpp"
#include "vertex.hpp"

#include <ranges>

namespace core {

void DrawLine(Context& t_Context,
              const glm::vec3& a,
              const glm::vec3& b,
              uint32_t t_Col) {
  glm::vec3 ab = b - a;
  float step = glm::fastInverseSqrt(ab.x * ab.x + ab.y * ab.y + ab.z * ab.z);

  for (float i = 0.f; i <= 1.f; i += step) {
    int x = a.x + ab.x * i, y = a.y + ab.y * i;

    if (x < 0 || y < 0 || x > t_Context.GetWidth() - 1 ||
        y > t_Context.GetHeight() - 1) {
      continue;
    }

    /*float z = glm::clamp(a.z + ab.z * i, 0.f, 1.f);
    if (t_Context.DepthBuffer[y * t_Context.GetWidth() + x] <= z) {
      continue;
    }*/

    t_Context.ColorBuffer[y * t_Context.GetWidth() + x] = t_Col;
    t_Context.DepthBuffer[y * t_Context.GetWidth() + x] = 0;
  }
}

void DrawWireframe(Context& t_Context,
                   const std::initializer_list<glm::vec3>& t_Points,
                   bool t_Closed,
                   uint32_t t_Col) {
  for (size_t i = 0; i < t_Points.size() - 1; i++) {
    DrawLine(t_Context, std::data(t_Points)[i], std::data(t_Points)[i + 1],
             t_Col);
  }

  if (t_Closed) {
    DrawLine(t_Context, std::data(t_Points)[t_Points.size() - 1],
             std::data(t_Points)[0], t_Col);
  }
}

void RenderTri(Context& t_Context,
               const Tri& t_Tri,
               const texture_t& t_Texture,
               float t_InverseFar,
               const shader_t& t_Shader) {
  const auto [a, b, c] = t_Tri;

  auto [min_x, max_x] = std::minmax({a.m_Pos.x, b.m_Pos.x, c.m_Pos.x});
  auto [min_y, max_y] = std::minmax({a.m_Pos.y, b.m_Pos.y, c.m_Pos.y});

  min_x = glm::clamp<int>(min_x, 0, t_Context.GetWidth() - 1);
  max_x = glm::clamp<int>(max_x, 0, t_Context.GetWidth() - 1);
  min_y = glm::clamp<int>(min_y, 0, t_Context.GetHeight() - 1);
  max_y = glm::clamp<int>(max_y, 0, t_Context.GetHeight() - 1);

  if (static_cast<unsigned>(max_x - min_x) *
          static_cast<unsigned>(max_y - min_y) ==
      0) {
    return;  // zero area triangle
  }

  const float InverseDoubleArea{1.f / DoubleTriangleArea(a.m_Pos.x, a.m_Pos.y,
                                                         b.m_Pos.x, b.m_Pos.y,
                                                         c.m_Pos.x, c.m_Pos.y)};
  const glm::vec3 Column1{
      glm::cross(glm::vec3(a.m_Pos.x, b.m_Pos.x, c.m_Pos.x),
                 glm::vec3(a.m_Pos.y, b.m_Pos.y, c.m_Pos.y))},
      Column2{b.m_Pos.y - c.m_Pos.y, c.m_Pos.y - a.m_Pos.y,
              a.m_Pos.y - b.m_Pos.y},
      Column3{c.m_Pos.x - b.m_Pos.x, a.m_Pos.x - c.m_Pos.x,
              b.m_Pos.x - a.m_Pos.x};
  const glm::mat3 MatBarycentric{Column1, Column2, Column3};

  for (int y = min_y; y <= max_y; y++) {
    int Row = y * t_Context.GetWidth();
    bool Outside{true};

    for (int x = min_x; x <= max_x; x++) {
      glm::vec3 Barycoord{MatBarycentric * glm::vec3(1, x, y)};

      if (Barycoord.x >= 0.f && Barycoord.y >= 0.f && Barycoord.z >= 0.f) {
        Outside = false;
        Barycoord *= InverseDoubleArea;

        float w = 1.f / (a.m_Pos.w * Barycoord.x + b.m_Pos.w * Barycoord.y +
                         c.m_Pos.w * Barycoord.z);
        float z =
            glm::clamp((a.m_Pos.z * Barycoord.x + b.m_Pos.z * Barycoord.y +
                        c.m_Pos.z * Barycoord.z) *
                           w * t_InverseFar,
                       0.f, 1.f);

        if (t_Context.DepthBuffer[Row + x] <= z) {
          continue;
        }

        float u = std::fabs(Barycoord.x * a.m_UV.x + Barycoord.y * b.m_UV.x +
                            Barycoord.z * c.m_UV.x) *
                  w;
        float v = std::fabs(Barycoord.x * a.m_UV.y + Barycoord.y * b.m_UV.y +
                            Barycoord.z * c.m_UV.y) *
                  w;

        float Light{glm::clamp(Barycoord.x * a.m_Light +
                                   Barycoord.y * b.m_Light +
                                   Barycoord.z * c.m_Light,
                               AMBIENT_INTENSITY, 1.f)};

        t_Shader(t_Context, t_Texture, glm::vec4(x, y, z, w), glm::vec2(u, v),
                 Light);
      }

      else {
        if (!Outside) {
          break;
        }
      }
    }
  }
}

/*void RenderTri(Context& t_Context,
               const Tri& t_Tri,
               const texture_t& t_Texture,
               float t_InverseFar,
               const shader_t& t_Shader) {
  const auto [a, b, c] = t_Tri;

  auto [min_x, max_x] = std::minmax({a.m_Pos.x, b.m_Pos.x, c.m_Pos.x});
  auto [min_y, max_y] = std::minmax({a.m_Pos.y, b.m_Pos.y, c.m_Pos.y});

  float full = 1.f / Edge(a.m_Pos.x, a.m_Pos.y, b.m_Pos.x, b.m_Pos.y, c.m_Pos.x,
                          c.m_Pos.y);

  min_x = glm::clamp<int>(min_x, 0, t_Context.GetWidth() - 1);
  max_x = glm::clamp<int>(max_x, 0, t_Context.GetWidth() - 1);
  min_y = glm::clamp<int>(min_y, 0, t_Context.GetHeight() - 1);
  max_y = glm::clamp<int>(max_y, 0, t_Context.GetHeight() - 1);

  if ((max_x - min_x) * (max_y - min_y) == 0) {
    return;  // zero area triangle
  }

  for (int y = min_y; y <= max_y; y++) {
    int Row = y * t_Context.GetWidth();
    bool Outside{true};

    for (int x = min_x; x <= max_x; x++) {
      float bc0 = Edge(b.m_Pos.x, b.m_Pos.y, c.m_Pos.x, c.m_Pos.y, x, y),
            bc1 = Edge(c.m_Pos.x, c.m_Pos.y, a.m_Pos.x, a.m_Pos.y, x, y),
            bc2 = Edge(a.m_Pos.x, a.m_Pos.y, b.m_Pos.x, b.m_Pos.y, x, y);

      if (bc0 <= 0.f && bc1 <= 0.f && bc2 <= 0.f) {
        Outside = false;
        bc0 *= full, bc1 *= full, bc2 *= full;

        float w = 1.f / (a.m_Pos.w * bc0 + b.m_Pos.w * bc1 + c.m_Pos.w * bc2);
        float z =
            glm::clamp((a.m_Pos.z * bc0 + b.m_Pos.z * bc1 + c.m_Pos.z * bc2) * w
* t_InverseFar, 0.f, 1.f);

        if (t_Context.DepthBuffer[Row + x] <= z) {
          continue;
        }

        float u = std::fabs(bc0 * a.m_UV.x + bc1 * b.m_UV.x + bc2 * c.m_UV.x) *
w, v = std::fabs(bc0 * a.m_UV.y + bc1 * b.m_UV.y + bc2 * c.m_UV.y) * w;

        float Light{
            glm::clamp(bc0 * a.m_Light + bc1 * b.m_Light + bc2 * c.m_Light,
                      AMBIENT_INTENSITY, 1.f)};

        t_Shader(t_Context, t_Texture, glm::vec4(x, y, z, w), glm::vec2(u, v),
                Light);
      }

      else {
        if (!Outside) {
          break;
        }
      }
    }
  }
}
*/

}  // namespace core
