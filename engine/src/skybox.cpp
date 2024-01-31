#include "skybox.hpp"

#include "camera.hpp"
#include "clipping.hpp"
#include "color.hpp"
#include "context.hpp"
#include "culling.hpp"
#include "mesh.hpp"
#include "object.hpp"
#include "render.hpp"
#include "srpch.hpp"
#include "texture.hpp"
#include "vertex.hpp"

namespace core {

std::vector<Tri> Skybox::Transform(const Camera& t_Camera,
                                   const Context& t_Context) {
  std::vector<Vertex> TransVertices{m_Mesh->GetVertices()};
  std::vector<Tri> TrianglesOut;

  glm::mat4 MatViewProjection{
      t_Camera.GetMatProjection(t_Context.GetWidth(), t_Context.GetHeight()) *
      t_Camera.GetMatView()};

  for (Vertex& Vertex : TransVertices) {
    Vertex.m_Pos = MatViewProjection *
                   glm::translate(glm::mat4(1.f), t_Camera.GetPos()) *
                   Vertex.m_Pos;
  }

  for (size_t ID = 0; ID < m_Mesh->GetIndices().size(); ID += 3) {
    std::vector<Tri> ClipTris{FrustumClipTriangle(
        Tri{TransVertices[ID], TransVertices[ID + 1], TransVertices[ID + 2]},
        t_Camera.GetFrustum())};

    for (Tri& Tri : ClipTris) {
      for (Vertex& Vertex : Tri) {
        Vertex.m_Pos.w = 1.f / Vertex.m_Pos.w;

        Vertex.m_Pos.x *= Vertex.m_Pos.w;
        Vertex.m_Pos.y *= Vertex.m_Pos.w;
        Vertex.m_Pos.z *= Vertex.m_Pos.w;
        Vertex.m_UV *= Vertex.m_Pos.w;

        Vertex.m_Pos.x = (Vertex.m_Pos.x + 1.f) * 0.5f * t_Context.GetWidth();
        Vertex.m_Pos.y = (Vertex.m_Pos.y + 1.f) * 0.5f * t_Context.GetHeight();
      }

      if (BackfaceCull(Tri)) {
        continue;
      }

      TrianglesOut.push_back(Tri);
    }
  }

  m_Timer.Tick();
  m_State = (std::cos(m_Timer.Elapsed() * 0.001f) + 1.f) * 0.5f;
  return TrianglesOut;
}

void Skybox::Render(const Camera& t_Camera, Context& t_Context) {
  const std::vector<Tri> RenderTris{Transform(t_Camera, t_Context)};

  for (const Tri& Tri : RenderTris) {
    RenderTri(t_Context, Tri);
  }
}

void Skybox::RenderTri(Context& t_Context, const Tri& t_Tri) {
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
    return;
  }

  for (int y = min_y; y <= max_y; y++) {
    int row = y * t_Context.GetWidth();

    bool Outside{true};

    for (int x = min_x; x <= max_x; x++) {
      if (t_Context.DepthBuffer[row + x] != INFINITY) {
        continue;
      }

      float bc0 = Edge(b.m_Pos.x, b.m_Pos.y, c.m_Pos.x, c.m_Pos.y, x, y),
            bc1 = Edge(c.m_Pos.x, c.m_Pos.y, a.m_Pos.x, a.m_Pos.y, x, y),
            bc2 = Edge(a.m_Pos.x, a.m_Pos.y, b.m_Pos.x, b.m_Pos.y, x, y);

      if (bc0 <= 0.f && bc1 <= 0.f && bc2 <= 0.f) {
        Outside = false;
        bc0 *= full, bc1 *= full, bc2 *= full;

        float w = 1.f / (a.m_Pos.w * bc0 + b.m_Pos.w * bc1 + c.m_Pos.w * bc2);
        float u =
            std::fabs(bc0 * a.m_UV.x + bc1 * b.m_UV.x + bc2 * c.m_UV.x) * w;
        float v =
            std::fabs(bc0 * a.m_UV.y + bc1 * b.m_UV.y + bc2 * c.m_UV.y) * w;

        t_Context.ColorBuffer[row + x] =
            m_Mesh->Texture->Sample(glm::vec2(u, v), 0.f);
      }

      else {
        if (!Outside) {
          break;
        }
      }
    }
  }
}

}  // namespace core
