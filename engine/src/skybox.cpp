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

namespace core
{

Skybox Skybox::New(uint32_t t_TextureID) {
    return Skybox(_M{.Mesh = LoadAsset(BUILTINS_DIR + "skybox.obj", t_TextureID), .Timer = Timer::New(), .State = 0.f});
}

triangle_vector_t Skybox::Transform(const Camera& t_Camera, buffer_t t_Buffer) const
{
    std::vector<Vertex> TransVertices{m.Mesh->GetVertices()};
    triangle_vector_t TrianglesOut;

    glm::mat4 MatViewProjection{t_Camera.GetMatProjection(t_Buffer->GetWidth(), t_Buffer->GetHeight()) *
                                t_Camera.GetMatView()};

    for (Vertex& Vertex : TransVertices)
    {
        Vertex.m_Pos = MatViewProjection * glm::translate(glm::mat4(1.f), t_Camera.GetPos()) * Vertex.m_Pos;
    }

    for (size_t ID = 0; ID < m.Mesh->GetIndices().size(); ID += 3)
    {
        triangle_t UnclippedTri{TransVertices[ID], TransVertices[ID + 1], TransVertices[ID + 2]};

        // only clip against near plane
        for (triangle_t& Tri : ClipTriangle(UnclippedTri, t_Camera.GetClipFrustum()[0]))
        {
            for (Vertex& Vertex : Tri)
            {
                Vertex.m_Pos.w = 1.f / Vertex.m_Pos.w;

                Vertex.m_Pos.x *= Vertex.m_Pos.w;
                Vertex.m_Pos.y *= Vertex.m_Pos.w;
                Vertex.m_Pos.z *= Vertex.m_Pos.w;
                Vertex.m_UV *= Vertex.m_Pos.w;

                Vertex.m_Pos.x = (Vertex.m_Pos.x + 1.f) * 0.5f * t_Buffer->GetWidth();
                Vertex.m_Pos.y = (Vertex.m_Pos.y + 1.f) * 0.5f * t_Buffer->GetHeight();
            }

            TrianglesOut.push_back(Tri);
        }
    }

    return TrianglesOut;
}

void Skybox::Render(const Camera& t_Camera, buffer_t t_Buffer) const
{
    const triangle_vector_t RenderTris = Transform(t_Camera, t_Buffer);

    #pragma omp parallel for
    for (const triangle_t& Tri : RenderTris)
    {
        RenderTri(t_Buffer, Tri);
    }
}

void Skybox::Update() {
    m.Timer.Tick();
    m.State = (std::cos(m.Timer.GetTimeElapsed() * 0.001f) + 1.f) * 0.5f;
}

void Skybox::RenderTri(buffer_t t_Buffer, const triangle_t& t_Tri) const
{
    auto [Min, Max] = GetBoundingBox(t_Tri);

    Min = glm::clamp(Min, glm::vec2(0.f), glm::vec2(t_Buffer->GetWidth() - 1, t_Buffer->GetHeight() - 1));
    Max = glm::clamp(Max, glm::vec2(0.f), glm::vec2(t_Buffer->GetWidth() - 1, t_Buffer->GetHeight() - 1));

    if ((Max.x - Max.x) * (Max.y - Min.y) == 0)
    {
        return;
    }

    auto [a, b, c] = t_Tri;
    float full = 1.f / Edge(a.m_Pos.x, a.m_Pos.y, b.m_Pos.x, b.m_Pos.y, c.m_Pos.x, c.m_Pos.y);

    for (int y = Min.y; y <= Max.y; y++)
    {
        int Row = y * t_Buffer->GetWidth();

        bool Outside{true};

        for (int x = Min.x; x <= Min.x; x++)
        {
            if (t_Buffer->GetDepth(Row + x) != INFINITY)
            {
                continue;
            }

            float bc0 = Edge(b.m_Pos.x, b.m_Pos.y, c.m_Pos.x, c.m_Pos.y, x, y),
                  bc1 = Edge(c.m_Pos.x, c.m_Pos.y, a.m_Pos.x, a.m_Pos.y, x, y),
                  bc2 = Edge(a.m_Pos.x, a.m_Pos.y, b.m_Pos.x, b.m_Pos.y, x, y);

            if (bc0 <= 0.f && bc1 <= 0.f && bc2 <= 0.f)
            {
                Outside = false;
                bc0 *= full, bc1 *= full, bc2 *= full;

                float w = 1.f / (a.m_Pos.w * bc0 + b.m_Pos.w * bc1 + c.m_Pos.w * bc2);
                float u = std::fabs(bc0 * a.m_UV.x + bc1 * b.m_UV.x + bc2 * c.m_UV.x) * w;
                float v = std::fabs(bc0 * a.m_UV.y + bc1 * b.m_UV.y + bc2 * c.m_UV.y) * w;

                t_Buffer->SetPixel(Row + x, GetTexture(m.Mesh->GetTextureID())->Sample(glm::vec2(u, v), 0.f));
            }

            else
            {
                if (!Outside)
                {
                    break;
                }
            }
        }
    }
}

/*
void Skybox::RenderTri(Context& t_Context, const Tri& t_Tri) {
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

  const float InverseDoubleArea = 1.f / DoubleTriangleArea(a.m_Pos.x, a.m_Pos.y,
                                                           b.m_Pos.x, b.m_Pos.y,
                                                           c.m_Pos.x, c.m_Pos.y);
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

      if (t_Context.DepthBuffer[Row + x] != INFINITY) {
        continue;
      }

      glm::vec3 Barycoord{MatBarycentric * glm::vec3(1, x, y)};

      if (Barycoord.x >= 0.f && Barycoord.y >= 0.f && Barycoord.z >= 0.f) {
        Outside = false;
        Barycoord *= InverseDoubleArea;

        float w = 1.f / (a.m_Pos.w * Barycoord.x + b.m_Pos.w * Barycoord.y +
                         c.m_Pos.w * Barycoord.z);

        float u = std::fabs(Barycoord.x * a.m_UV.x + Barycoord.y * b.m_UV.x +
                            Barycoord.z * c.m_UV.x) * w;

        float v = std::fabs(Barycoord.x * a.m_UV.y + Barycoord.y * b.m_UV.y +
                            Barycoord.z * c.m_UV.y) * w;

        t_Context.ColorBuffer[Row + x] = m_Mesh->Texture->Sample(glm::vec2(u, v), 0.f);
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

} // namespace core
