#include "application.hpp"

#include "callbacks.hpp"
#include "culling.hpp"
#include "keys.hpp"
#include "light.hpp"
#include "mesh.hpp"
#include "object.hpp"
#include "render.hpp"
#include "srpch.hpp"
#include "texture.hpp"
#include "vertex.hpp"

namespace core {

Application* Application::GetInstance() {
  if (!m_Instance) {
    m_Instance = new Application();
  }
  return m_Instance;
}

void Application::Init() {
  LoadMap();

  SDL_AddEventWatch(ActionCallback, this);
  // SDL_SetRelativeMouseMode(SDL_TRUE);
}

void Application::LogPerformance() {
  std::clog << "Avrg. Perf: " << m_Timer.Performance() << "ms/fr, "
            << m_Timer.Elapsed() << "ms Elapsed\n";
}

void Application::Run() {
  while (m_Running) {
    SDL_PumpEvents();
    m_Camera.HandleMovement();
    m_Camera.HandleRotation();

    glm::mat4 MatView{m_Camera.GetMatView()};
    glm::mat4 MatProjection{
        m_Camera.GetMatProjection(m_Context.GetWidth(), m_Context.GetHeight())};

    std::vector<PointLight> ViewspacePointLights;
    std::transform(m_PointLights.begin(), m_PointLights.end(),
                   std::back_inserter(ViewspacePointLights),
                   [MatView](const PointLight& Light) {
                     return PointLight(MatView * glm::vec4(Light.GetPos(), 1.f),
                                       Light.GetIntensity());
                   });

    for (const Object& Object : m_Objects) {
      // Object.SetAngle(
      //     glm::vec3(m_Camera.GetAngle().y, 0.f, m_Camera.GetAngle().x));

      glm::mat4 MatModel{Object.GetTranslation() * Object.GetRotation() *
                         Object.GetScale()};
      glm::mat4 MatModelView{MatView * MatModel};

      // std::vector<Tri> DrawTris;

      for (const Mesh& Mesh : Object.GetMeshes()) {
        std::vector<Vertex> TransVertices{Mesh.GetVertices()};

        for (Vertex& Vertex : TransVertices) {
          Vertex.m_Pos = MatModelView * Vertex.m_Pos;
          Vertex.m_Normal = Object.GetRotation() * Vertex.m_Normal;

          for (const PointLight& Light : ViewspacePointLights) {
            Vertex.m_Light += Light.GetLighting(Vertex);
          }

          Vertex.m_Pos = MatProjection * Vertex.m_Pos;
        }

        for (size_t ID = 0; ID < Mesh.GetIndices().size(); ID += 3) {
          std::vector<Tri> ClipTris{
              FrustumClipTriangle(Tri{TransVertices[ID], TransVertices[ID + 1],
                                      TransVertices[ID + 2]},
                                  m_Camera.GetFrustum())};

          for (Tri& Tri : ClipTris) {
            for (Vertex& Vertex : Tri) {
              Vertex.m_Pos.w = 1.f / Vertex.m_Pos.w;

              Vertex.m_Pos.x *= Vertex.m_Pos.w;
              Vertex.m_Pos.y *= Vertex.m_Pos.w;
              Vertex.m_Pos.z *= Vertex.m_Pos.w;
              Vertex.m_UV *= Vertex.m_Pos.w;

              Vertex.m_Pos.x =
                  (Vertex.m_Pos.x + 1.f) * 0.5f * m_Context.GetWidth();
              Vertex.m_Pos.y =
                  (Vertex.m_Pos.y + 1.f) * 0.5f * m_Context.GetHeight();
            }

            if (BackfaceCull(Tri)) {
              continue;
            }

            if (!m_Wireframe) {
              RenderTri(m_Context, Tri, Mesh.GetTexName(),
                        m_Camera.GetInverseFar());
            } else {
              DrawWireframe(m_Context,
                            {Tri[0].m_Pos, Tri[1].m_Pos, Tri[2].m_Pos});
            }
          }
        }
      }

      /*std::sort(DrawTris.begin(), DrawTris.end(),
                [](const Tri& t_First, const Tri& t_Second) {
                  return (t_First[0].m_Pos.z + t_First[1].m_Pos.z +
                          t_First[2].m_Pos.z) *
                             glm::third<float>() >
                         (t_Second[0].m_Pos.z + t_Second[1].m_Pos.z +
                          t_Second[2].m_Pos.z) *
                             glm::third<float>();
                });

      for (const Tri& Tri : DrawTris) {
        RenderTri(m_Context, Tri, "../../engine/builtins/untextured.png",
                  m_Camera.GetInverseFar());
      }*/
    }

    m_Skybox.Render(m_Camera, m_Context);

    m_Context.Update();
    m_Timer.Tick();
  }
}

}  // namespace core
