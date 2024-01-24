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
  if (!IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF)) {
    throw std::runtime_error("SDL image init failure.");
  }

  m_Objects.push_back(
      Object(LoadMeshOBJ("../../resources/meshes/", "teapot.obj"),
             glm::vec3(0.f, 0.f, -1.f)));

  SDL_SetRelativeMouseMode(SDL_TRUE);
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

      if (event.type == SDL_MOUSEMOTION) {
        int DeltaMouseX, DeltaMouseY;
        SDL_GetRelativeMouseState(&DeltaMouseX, &DeltaMouseY);

        float DMX{DeltaMouseX /
                  static_cast<float>(m_Context.GetWidth() * 0.5f)};
        float DMY{DeltaMouseY /
                  static_cast<float>(m_Context.GetHeight() * 0.5f)};

        const float DeltaAngleX{DMX * SENSITIVITY_X},
            DeltaAngleY{DMY * SENSITIVITY_Y};

        glm::vec3 NewAngle{
            glm::clamp<float>(m_Camera.GetAngle().x + DeltaAngleY,
                              -glm::radians<float>(CAM_MAX_VERTICAL_ANGLE),
                              glm::radians<float>(CAM_MAX_VERTICAL_ANGLE)),
            m_Camera.GetAngle().y + DeltaAngleX, m_Camera.GetAngle().z};

        std::cout << DeltaAngleX << ' ' << DeltaAngleY << '\n';

        // basic average smoothing
        m_Camera.SetAngle((m_Camera.GetAngle() + NewAngle) * 0.5f);
      }
    }

    glm::mat4 MatView{m_Camera.GetMatView()};
    glm::mat4 MatProjection{
        m_Camera.GetMatProjection(m_Context.GetWidth(), m_Context.GetHeight())};

    for (const Object& Object : m_Objects) {
      glm::mat4 MatModel{Object.GetTranslation() * Object.GetRotation() *
                         Object.GetScale()};
      glm::mat4 MatModelView{MatView * MatModel};

      for (const Mesh& Mesh : Object.GetMeshes()) {
        std::vector<Vertex> TransVertices{Mesh.GetVertices()};

        for (Vertex& Vertex : TransVertices) {
          Vertex.m_Pos = MatProjection * MatModelView * Vertex.m_Pos;
          Vertex.m_Normal = Object.GetRotation() * Vertex.m_Normal;
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
              RenderTri(m_Context, Tri, Mesh.GetTexName());
            } else {
              DrawLine(m_Context, Tri[0].m_Pos, Tri[1].m_Pos);
              DrawLine(m_Context, Tri[1].m_Pos, Tri[2].m_Pos);
              DrawLine(m_Context, Tri[2].m_Pos, Tri[0].m_Pos);
            }
          }
        }
      }
    }

    m_Skybox.Render(m_Camera, m_Context);

    m_Context.Update();
    m_Timer.Tick();
  }
}

}  // namespace core
