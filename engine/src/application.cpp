#include "application.hpp"
#include "culling.hpp"
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
  m_Objects.push_back(
      LoadAssetOBJ("../../../builtins/cube.obj"));  // m4a1/m4a1.obj"));
  LoadTexture("icon.png");
}

void Application::LogPerformance() {
  std::clog << "Avrg. Perf: " << m_Timer.Performance() << "ms/fr, "
            << m_Timer.Elapsed() << "ms Elapsed\n";
}

void Application::Run() {
  while (m_Running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        m_Running = false;
      }

      if (event.type == SDL_WINDOWEVENT_RESIZED) {
        SDL_DisplayMode Spec{m_Context.GetSpec()};
        SDL_Rect Viewport{m_Context.GetViewport()};

        Spec.w = event.window.data1;
        Spec.h = event.window.data2;
        m_Context.SetSpec(Spec);

        m_Context.SetViewport(Viewport.x, Viewport.y, Spec.w, Spec.h);
      }

      if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_ESCAPE) {
          m_Wireframe = !m_Wireframe;
        }

        if (event.key.keysym.sym == SDLK_RETURN) {
          m_Paused = !m_Paused;
        }

        if (event.key.keysym.sym == SDLK_q) {
          m_Running = false;
        }

        if (event.key.keysym.sym == SDLK_w) {
          m_Camera.SetPos(m_Camera.GetPos() +
                          glm::vec3(0.f, 0.f, -1.f) * 0.015f);
        }
        if (event.key.keysym.sym == SDLK_s) {
          m_Camera.SetPos(m_Camera.GetPos() +
                          glm::vec3(0.f, 0.f, 1.f) * 0.015f);
        }

        if (event.key.keysym.sym == SDLK_a) {
          m_Camera.SetPos(m_Camera.GetPos() +
                          glm::vec3(-1.f, 0.f, 0.f) * 0.015f);
        }
        if (event.key.keysym.sym == SDLK_d) {
          m_Camera.SetPos(m_Camera.GetPos() +
                          glm::vec3(1.f, 0.f, 0.f) * 0.015f);
        }
      }
    }

    glm::mat4 MatView{m_Camera.GetMatView()};
    glm::mat4 MatProjection{
        m_Camera.GetMatProjection(m_Context.GetWidth(), m_Context.GetHeight())};

    glm::vec4 Viewport{m_Context.GetViewport().x, m_Context.GetViewport().y,
                       m_Context.GetViewport().w, m_Context.GetViewport().h};

    for (const Object& Object : m_Objects) {
      glm::mat4 MatModel{
          Object.GetTranslation() *
          Object.GetRotation(!m_Paused ? m_Timer.Elapsed() * 0.001f : 0.f) *
          Object.GetScale()};
      glm::mat4 MatModelView{MatView * MatModel};

      for (const Mesh& Mesh : Object.GetMeshes()) {
        std::vector<Vertex> TransVertices{Mesh.GetVertices()};

        for (Vertex& Vertex : TransVertices) {
          Vertex.m_Pos = MatProjection * MatModelView * Vertex.m_Pos;
          Vertex.m_Normal =
              Object.GetRotation(m_Timer.Elapsed() * 0.001f) * Vertex.m_Normal;
        }

        for (size_t ID = 0; ID < Mesh.GetIndices().size(); ID += 3) {
          Tri TransTri{TransVertices[ID], TransVertices[ID + 1],
                       TransVertices[ID + 2]};

          std::vector<Tri> ClipTris{
              FrustumClipTriangle(TransTri, m_Camera.GetFrustum())};

          for (core::Tri& Tri : ClipTris) {
            for (Vertex& Vertex : Tri) {
              Vertex.m_Pos.w = 1.f / Vertex.m_Pos.w;

              Vertex.m_Pos.x *= Vertex.m_Pos.w;
              Vertex.m_Pos.y *= Vertex.m_Pos.w;
              Vertex.m_Pos.z *= Vertex.m_Pos.w;
              Vertex.m_UV *= Vertex.m_Pos.w;

              Vertex.m_Pos.x = glm::clamp(Vertex.m_Pos.x + 1.f, 0.f, 2.f) *
                               m_Context.GetWidth() * 0.5f;
              Vertex.m_Pos.y = glm::clamp(Vertex.m_Pos.y + 1.f, 0.f, 2.f) *
                               m_Context.GetHeight() * 0.5f;
            }

            if (BackfaceCull(Tri)) {
              continue;
            }

            if (!m_Wireframe) {
              RenderTri(m_Context, Tri, Mesh.GetTexName());
            } else {
              DrawLine(m_Context, Tri[0].m_Pos, Tri[1].m_Pos);
              DrawLine(m_Context, Tri[1].m_Pos, Tri[2].m_Pos);
              DrawLine(m_Context, Tri[2].m_Pos, Tri[0].m_Pos);
            };
          }
        }
      }
    }

    m_Context.Update();
    m_Timer.Tick();
  }
}

}  // namespace core
