#include "render.hpp"

#include "color.hpp"
#include "context.hpp"
#include "glm/gtc/packing.hpp"
#include "srpch.hpp"
#include "texture.hpp"
#include "vertex.hpp"

namespace core {

void DrawLine(Context& t_Context,
              const glm::vec3& a,
              const glm::vec3& b,
              uint32_t col) {
  glm::vec3 ab = b - a;
  float step = glm::fastInverseSqrt(ab.x * ab.x + ab.y * ab.y + ab.z * ab.z);

  for (float i = 0.f; i <= 1.f; i += step) {
    int x = a.x + ab.x * i, y = a.y + ab.y * i;

    if (x < 0 || y < 0 || x > t_Context.GetWidth() - 1 ||
        y > t_Context.GetHeight() - 1) {
      continue;
    }

    float z = glm::clamp(a.z + ab.z * i, 0.f, 1.f);

    if (t_Context.DepthBuffer[y * t_Context.GetWidth() + x] <= z) {
      continue;
    }

    t_Context.ColorBuffer[y * t_Context.GetWidth() + x] = col;
    t_Context.DepthBuffer[y * t_Context.GetWidth() + x] = z;
  }
}

float Edge(float ax, float ay, float bx, float by, float cx, float cy) {
  return (cx - ax) * (by - ay) - (cy - ay) * (bx - ax);
}

void RenderTri(Context& t_Context,
               const Tri& t_Tri,
               const std::string& t_TexName) {
  const auto [a, b, c] = t_Tri;
  const texture_t Texture{GetTexture(t_TexName)};

  auto [min_x, max_x] = std::minmax({a.m_Pos.x, b.m_Pos.x, c.m_Pos.x});
  auto [min_y, max_y] = std::minmax({a.m_Pos.y, b.m_Pos.y, c.m_Pos.y});

  float full = 1.f / Edge(a.m_Pos.x, a.m_Pos.y, b.m_Pos.x, b.m_Pos.y, c.m_Pos.x,
                          c.m_Pos.y);

  min_x = glm::clamp<int>(min_x, 0, t_Context.GetWidth() - 1);
  max_x = glm::clamp<int>(max_x, 0, t_Context.GetWidth() - 1);
  min_y = glm::clamp<int>(min_y, 0, t_Context.GetHeight() - 1);
  max_y = glm::clamp<int>(max_y, 0, t_Context.GetHeight() - 1);

  if ((max_x - min_x) * (max_y - min_y) == 0) {
    return;
  }

  /*
  DrawLine(t_Context, glm::vec3(min_x, min_y, 1.f),
           glm::vec3(max_x, min_y, 1.f));
  DrawLine(t_Context, glm::vec3(max_x, min_y, 1.f),
           glm::vec3(max_x, max_y, 1.f));
  DrawLine(t_Context, glm::vec3(max_x, max_y, 1.f),
           glm::vec3(min_x, max_y, 1.f));
  DrawLine(t_Context, glm::vec3(min_x, max_y, 1.f),
           glm::vec3(min_x, min_y, 1.f));
  */

  for (int y = min_y; y <= max_y; y++) {
    int row = y * t_Context.GetWidth();

    for (int x = min_x; x <= max_x; x++) {
      float bc0 = Edge(b.m_Pos.x, b.m_Pos.y, c.m_Pos.x, c.m_Pos.y, x, y),
            bc1 = Edge(c.m_Pos.x, c.m_Pos.y, a.m_Pos.x, a.m_Pos.y, x, y),
            bc2 = Edge(a.m_Pos.x, a.m_Pos.y, b.m_Pos.x, b.m_Pos.y, x, y);

      if (bc0 <= 0.f && bc1 <= 0.f && bc2 <= 0.f) {
        bc0 *= full, bc1 *= full, bc2 *= full;

        float W = 1.f / (a.m_Pos.w * bc0 + b.m_Pos.w * bc1 + c.m_Pos.w * bc2);
        float Z =
            glm::clamp((a.m_Pos.z * bc0 + b.m_Pos.z * bc1 + c.m_Pos.z * bc2) *
                           W * (1.f / 10.f),
                       0.f, 1.f);

        if (t_Context.DepthBuffer[row + x] <= Z) {
          continue;
        }

        float u = std::fabs(bc0 * a.m_UV.x + bc1 * b.m_UV.x + bc2 * c.m_UV.x) *
                  W,
              v = std::fabs(bc0 * a.m_UV.y + bc1 * b.m_UV.y + bc2 * c.m_UV.y) *
                  W;

        uint32_t Pixel{
            Texture->Data[int(Texture->GetHeight() *
                              glm::clamp(v - static_cast<long>(v), 0.f, 1.f)) *
                              Texture->GetWidth() +
                          int(Texture->GetWidth() *
                              glm::clamp(u - static_cast<long>(u), 0.f, 1.f))]};

        t_Context.ColorBuffer[row + x] = Pixel;
        // t_Context.ColorBuffer[row + x] =
        //    ToUint32(1.f - Z, 1.f - Z, 1.f - Z, 1.f);
        t_Context.DepthBuffer[row + x] = Z;
      }
    }
  }
}

}  // namespace core
