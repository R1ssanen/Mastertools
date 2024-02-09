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

namespace core
{

void App::Init()
{
    LoadMap();
    SDL_AddEventWatch(ActionCallback, this);
    SDL_SetRelativeMouseMode(SDL_TRUE);
}

void App::Run()
{
    while (m_Running)
    {
        SDL_PumpEvents();
        m_Camera.HandleMovement();
        m_Camera.HandleRotation();

        glm::mat4 MatView = m_Camera.GetMatView();
        glm::mat4 MatProjection = m_Camera.GetMatProjection(m_Context.GetWidth(), m_Context.GetHeight());
        glm::mat4 MatViewProjection = MatProjection * MatView;

        std::vector<DrawTri> TransparentTris;

        for (const Object& Object : m_Objects)
        {
            glm::mat4 MatModel = Object.GetTranslation() * Object.GetRotation() * Object.GetScale();

            for (const mesh_t& Mesh : Object.GetMeshes())
            {
                std::vector<Vertex> TransVertices = Mesh->GetVertices();

                for (Vertex& Vertex : TransVertices)
                {
                    Vertex.m_Pos = MatModel * Vertex.m_Pos;
                    Vertex.m_Normal = glm::normalize(Object.GetRotation() * Vertex.m_Normal);

                    for (const PointLight& Light : m_PointLights)
                    {
                        glm::vec4 LightDirection = glm::vec4(Light.GetPos(), 1.f) - Vertex.m_Pos;
                        float Lambert = glm::dot(Vertex.m_Normal, LightDirection);

                        Vertex.m_Light += Light.GetLighting(Vertex) * ((Lambert + 1.f) * 0.5f);
                    }

                    Vertex.m_Pos = MatViewProjection * Vertex.m_Pos;
                }

                for (size_t ID = 0; ID < Mesh->GetIndices().size(); ID += 3)
                {
                    triangle_t UnclippedTri = {TransVertices[ID], TransVertices[ID + 1], TransVertices[ID + 2]};

                    for (triangle_t& Tri : FrustumClipTriangle(UnclippedTri, m_Camera.GetFrustum()))
                    {
                        for (Vertex& Vertex : Tri)
                        {
                            Vertex.m_Pos.w = 1.f / Vertex.m_Pos.w;
                            Vertex.m_Pos.x *= Vertex.m_Pos.w;
                            Vertex.m_Pos.y *= Vertex.m_Pos.w;
                        }

                        if (!Mesh->Texture->IsDoublesided() && ClipspaceBackfaceCull(Tri))
                        {
                            continue;
                        }

                        for (Vertex& Vertex : Tri)
                        {
                            Vertex.m_Pos.z *= Vertex.m_Pos.w;
                            Vertex.m_UV *= Vertex.m_Pos.w;

                            Vertex.m_Pos.x = (Vertex.m_Pos.x + 1.f) * 0.5f * m_Context.GetWidth();
                            Vertex.m_Pos.y = (Vertex.m_Pos.y + 1.f) * 0.5f * m_Context.GetHeight();
                        }

                        if (GetWireframe())
                        {
                            DrawWireframe(m_Context, {Tri[0].m_Pos, Tri[1].m_Pos, Tri[2].m_Pos});
                        }

                        else if (Mesh->Texture->IsTransparent())
                        {
                            TransparentTris.push_back(DrawTri{Tri, Mesh->Texture});
                        }

                        else
                        {
                            RenderTriBary(m_Context, Tri, Mesh->Texture, m_Camera.GetInverseFar(), OpaqueSTD);
                        }
                    }
                }
            }
        }

        /*const mesh_t LightOverlay{core::GetDefaultLightOverlay()};
        for (const PointLight& Light : m_PointLights) {
          std::vector<Vertex> TransVertices{LightOverlay->GetVertices()};

          glm::mat4 MatModel{glm::translate(m_Camera.GetMatLookAt(Light.GetPos()),
                                            Light.GetPos())};

          for (Vertex& Vertex : TransVertices) {
            Vertex.m_Pos = MatViewProjection * MatModel * Vertex.m_Pos;
            Vertex.m_Light = 1.f;
          }

          for (size_t ID = 0; ID < LightOverlay->GetIndices().size(); ID += 3) {
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

              if (m_Wireframe) {
                DrawWireframe(m_Context, {Tri[0].m_Pos, Tri[1].m_Pos, Tri[2].m_Pos},
                              true, 0xFF0000FF);
              }

              else {
                TransparentTris.push_back(
                    DrawTri{Tri, LightOverlay->Texture->GetName()});
              }
            }
          }
        }

        */

        //m_Skybox.Render(m_Camera, m_Context);

        std::sort(TransparentTris.begin(), TransparentTris.end(), DrawTri::FarToClose);
        for (const DrawTri& Tri : TransparentTris)
        {
            RenderTriBary(m_Context, Tri.m_Tri, Tri.m_Texture, m_Camera.GetInverseFar(), TransparentSTD);
        }

        m_Context.Update();
        m_Timer.Tick();
        m_Context.Clear();
    }
}

} // namespace core
