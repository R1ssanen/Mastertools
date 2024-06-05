#include "skybox.hpp"

#include "../mtpch.hpp"
#include "../render/context.hpp"
#include "../render/render.hpp"
#include "../resource/mesh.hpp"
#include "../resource/object.hpp"
#include "../resource/texture.hpp"
#include "camera.hpp"
#include "clipping.hpp"
#include "color.hpp"
#include "culling.hpp"
#include "triangle.hpp"
#include "vertex.hpp"

namespace mt {

    Skybox::Skybox(u32 TextureID)
        : m(_M{ .Mesh    = LoadAsset(BUILTINS_DIR + "skybox.obj", 0),
                .Texture = GetTexture(TextureID),
                .State   = 0.f }) { }

    void Skybox::Transform(
        const Camera& Camera, buffer_t Buffer, triangle_vector_t& o_TransformedTriangles
    ) const {
        vertex_vector_t TransVertices     = m.Mesh->GetVertices();

        glm::mat4       MatView           = Camera.GetMatView();
        glm::mat4       MatProjection     = Camera.GetMatProjection();
        glm::mat4       MatViewProjection = MatProjection * MatView;

        math::Frustum   CameraFrustum     = Camera.GetFrustum();
        math::Frustum FrustumNoFar = math::Frustum(CameraFrustum.begin(), CameraFrustum.end() - 1);

        for (Vertex& Vertex : TransVertices) {
            Vertex.Pos = MatView * glm::translate(glm::mat4(1.f), Camera.GetPos()) * Vertex.Pos;
        }

        for (Triangle& Tri : ConstructTriangles(m.Mesh, TransVertices)) {
            triangle_vector_t ClippedTriangles;
            FrustumClipTriangle(Tri, FrustumNoFar, ClippedTriangles);

            for (Triangle& ClippedTri : ClippedTriangles) {
                for (Vertex& Vertex : ClippedTri.m_Vertices) {
                    Vertex.Pos   = MatProjection * Vertex.Pos;

                    Vertex.Pos.w = 1.f / Vertex.Pos.w;
                    Vertex.Pos.x *= Vertex.Pos.w;
                    Vertex.Pos.y *= Vertex.Pos.w;

                    Vertex.UV *= Vertex.Pos.w;
                    Vertex.Pos.x = (Vertex.Pos.x + 1.f) * 0.5f * Buffer->GetWidth();
                    Vertex.Pos.y = (Vertex.Pos.y + 1.f) * 0.5f * Buffer->GetHeight();
                }
            }

            o_TransformedTriangles.insert(
                o_TransformedTriangles.end(), ClippedTriangles.begin(), ClippedTriangles.end()
            );
        }
    }

    void Skybox::Render(const Camera& Camera, buffer_t Buffer) const {
        triangle_vector_t RenderTriangles;
        Transform(Camera, Buffer, RenderTriangles);

        std::for_each(
            RenderTriangles.begin(), RenderTriangles.end(),
            [this, Buffer](const Triangle& Tri) { RenderTri(Buffer, Tri); }
        );

        /*auto Render = [this, Buffer] (const Triangle& Tri) {
            RenderTri(Buffer, Tri);
        };


        std::vector<std::thread> Jobs;
        for (const Triangle& Tri : RenderTriangles) {
            Jobs.emplace_back(Render, std::cref(Tri));
        }

        for (auto& Job : Jobs) {Job.join();}*/
    }

    void Skybox::Update() { return; }

    void Skybox::RenderTri(buffer_t Buffer, const Triangle& Tri) const {
        auto [Min, Max] = Tri.GetBoundingBox();

        Min             = glm::clamp(
            Min, glm::vec2(0.f), glm::vec2(Buffer->GetWidth() - 1, Buffer->GetHeight() - 1)
        );
        Max = glm::clamp(
            Max, glm::vec2(0.f), glm::vec2(Buffer->GetWidth() - 1, Buffer->GetHeight() - 1)
        );

        if ((Max.x - Min.x) * (Max.y - Min.y) == 0.f) { return; }

        auto [a, b, c]         = Tri.m_Vertices;
        f32 InverseDeterminant = 1.f / Edge(a.Pos.x, a.Pos.y, b.Pos.x, b.Pos.y, c.Pos.x, c.Pos.y);

        for (i32 y = Min.y; y <= Max.y; ++y) {
            i32 Row     = y * Buffer->GetWidth();
            b8  Outside = true;

            for (i32 x = Min.x; x <= Max.x; ++x) {
                if (Buffer->GetDepth(Row + x) != 1.f) { continue; }

                f32 bc0 = Edge(b.Pos.x, b.Pos.y, c.Pos.x, c.Pos.y, x, y),
                    bc1 = Edge(c.Pos.x, c.Pos.y, a.Pos.x, a.Pos.y, x, y),
                    bc2 = Edge(a.Pos.x, a.Pos.y, b.Pos.x, b.Pos.y, x, y);

                if (bc0 < 0.f && bc1 < 0.f && bc2 < 0.f) {
                    Outside = false;

                    bc0 *= InverseDeterminant;
                    bc1 *= InverseDeterminant;
                    bc2 *= InverseDeterminant;

                    f32 w = 1.f / (a.Pos.w * bc0 + b.Pos.w * bc1 + c.Pos.w * bc2);
                    f32 u = (a.UV.x * bc0 + b.UV.x * bc1 + c.UV.x * bc2) * w;
                    f32 v = (a.UV.y * bc0 + b.UV.y * bc1 + c.UV.y * bc2) * w;

                    Buffer->SetPixel(Row + x, m.Texture->Sample(glm::vec2(u, v), 0.f));
                }

                else {
                    if (!Outside) { break; }
                }
            }
        }
    }

    /*
    void Skybox::RenderTri(Context& Context, const Tri& Tri) {
      const auto [a, b, c] = Tri;

      auto [min_x, max_x] = std::minmax({a.m_Pos.x, b.m_Pos.x, c.m_Pos.x});
      auto [min_y, max_y] = std::minmax({a.m_Pos.y, b.m_Pos.y, c.m_Pos.y});

      min_x = glm::clamp<i32>(min_x, 0, Context.GetWidth() - 1);
      max_x = glm::clamp<i32>(max_x, 0, Context.GetWidth() - 1);
      min_y = glm::clamp<i32>(min_y, 0, Context.GetHeight() - 1);
      max_y = glm::clamp<i32>(max_y, 0, Context.GetHeight() - 1);

      if (static_cast<u32>(max_x - min_x) *
              static_cast<u32>(max_y - min_y) ==
          0) {
        return;  // zero area triangle
      }

      const f32 InverseDoubleArea = 1.f / DoubleTriangleArea(a.m_Pos.x, a.m_Pos.y,
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

      for (i32 y = min_y; y <= max_y; ++y) {
        i32 Row = y * Context.GetWidth();
        b8 Outside{true};

        for (i32 x = min_x; x <= max_x; ++x) {

          if (Context.DepthBuffer[Row + x] != INFINITY) {
            continue;
          }

          glm::vec3 Barycoord{MatBarycentric * glm::vec3(1, x, y)};

          if (Barycoord.x >= 0.f && Barycoord.y >= 0.f && Barycoord.z >= 0.f) {
            Outside = false;
            Barycoord *= InverseDoubleArea;

            f32 w = 1.f / (a.m_Pos.w * Barycoord.x + b.m_Pos.w * Barycoord.y +
                             c.m_Pos.w * Barycoord.z);

            f32 u = std::fabs(Barycoord.x * a.UV.x + Barycoord.y * b.UV.x +
                                Barycoord.z * c.UV.x) * w;

            f32 v = std::fabs(Barycoord.x * a.UV.y + Barycoord.y * b.UV.y +
                                Barycoord.z * c.UV.y) * w;

            Context.ColorBuffer[Row + x] = m_Mesh->Texture->Sample(glm::vec2(u, v), 0.f);
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

} // namespace mt
