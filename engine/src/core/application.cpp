#include "application.hpp"

#include "../mtpch.hpp"
#include "../render/render.hpp"
#include "../resource/mesh.hpp"
#include "../resource/object.hpp"
#include "../resource/texture.hpp"
#include "callbacks.hpp"
#include "clipping.hpp"
#include "culling.hpp"
#include "emitter.hpp"
#include "keys.hpp"
#include "light.hpp"
#include "triangle.hpp"
#include "vertex.hpp"

namespace mt {

    void App::Init() {
        srand(std::chrono::system_clock::now().time_since_epoch().count());
        SDL_AddEventWatch(ActionCallback, this);
        SDL_SetRelativeMouseMode(SDL_TRUE);
    }

    void App::Run() {

        for (; m_Running; SDL_PumpEvents()) {
            m_ActiveScene.Update(m_Timer.GetTimeElapsed() * 0.001f);

            glm::mat4         MatView       = m_ActiveScene.GetCamera().GetMatView();
            glm::mat4         MatProjection = m_ActiveScene.GetCamera().GetMatProjection();

            triangle_vector_t OpaqueQueue, TransparentQueue;

            for (const Object& Object : m_ActiveScene.GetObjects()) {
                glm::mat4 MatModel = Object.GetMatModel();

                for (mesh_t Mesh : Object.GetMeshes()) {

                    math::VolumeIntersectionFlag Flag =
                        FrustumCullBoundingSphere(m_ActiveScene.GetCamera(), Mesh, Object.GetPos());

                    if (Flag == math::VolumeIntersectionFlag::OUT) { continue; }

                    // u32  DebugColor = GetDebugCullColor(Flag);
                    // auto Texture    = ColorTexture::New(std::to_string(DebugColor), DebugColor);
                    texture_t       Texture       = GetTexture(Mesh->GetTextureID());

                    vertex_vector_t TransVertices = Mesh->GetVertices();
                    for (Vertex& Vertex : TransVertices) {
                        Vertex.Pos = MatModel * Vertex.Pos;
                        Vertex.Normal =
                            glm::normalize(MatModel * glm::vec4(glm::vec3(Vertex.Normal), 0.f));

                        for (const PointLight& Light : m_ActiveScene.GetLights()) {
                            const glm::vec4 LightDirection =
                                glm::normalize(glm::vec4(Light.GetPos(), 1.f) - Vertex.Pos);
                            f32 Lambert = glm::dot(Vertex.Normal, LightDirection);

                            Vertex.Light += Light.GetLighting(Vertex) * ((Lambert + 1.f) * 0.5f);
                        }

                        Vertex.Pos = MatView * Vertex.Pos;
                    }

                    for (Triangle& Tri : ConstructTriangles(Mesh, TransVertices)) {
                        triangle_vector_t ClippedTriangles;

                        if (Flag == math::VolumeIntersectionFlag::INTERSECTS) {
                            FrustumClipTriangle(
                                Tri, m_ActiveScene.GetCamera().GetFrustum(), ClippedTriangles
                            );
                        } else {
                            ClippedTriangles = { Tri };
                        }

                        for (Triangle& ClippedTri : ClippedTriangles) {
                            for (Vertex& Vertex : ClippedTri.m_Vertices) {
                                Vertex.Pos   = MatProjection * Vertex.Pos;

                                Vertex.Pos.w = 1.f / Vertex.Pos.w;
                                Vertex.Pos.z *= Vertex.Pos.w;
                                Vertex.UV *= Vertex.Pos.w;

                                Vertex.Pos.x *= Vertex.Pos.w;
                                Vertex.Pos.y *= Vertex.Pos.w;

                                Vertex.Pos.x = (Vertex.Pos.x + 1.f) * 0.5f;
                                Vertex.Pos.y = (Vertex.Pos.y + 1.f) * 0.5f;
                            }

                            if (!Texture->IsDoublesided() && ClipspaceBackfaceCull(ClippedTri)) {
                                continue;
                            }

                            ClippedTri.CalculateAverageDepth();
                            ClippedTri.m_TextureID = Texture->GetID();

                            if (Texture->IsTransparent()) {
                                TransparentQueue.push_back(std::move(ClippedTri));
                            }

                            else {
                                OpaqueQueue.push_back(std::move(ClippedTri));
                            }
                        }
                    }
                }
            }

            std::sort(OpaqueQueue.begin(), OpaqueQueue.end(), std::less());
            for (Triangle& Tri : OpaqueQueue) {
                RenderTriBary(
                    m_Context.GetBuffer(), Tri, m_ActiveScene.GetCamera().GetInverseFar()
                );
            }

            if (m_ActiveScene.GetSkybox().has_value()) {
                m_ActiveScene.GetSkybox().value().Render(
                    m_ActiveScene.GetCamera(), m_Context.GetBuffer()
                );
            }

            std::sort(TransparentQueue.begin(), TransparentQueue.end(), std::greater());
            for (Triangle& Tri : TransparentQueue) {
                RenderTriBary(
                    m_Context.GetBuffer(), Tri, m_ActiveScene.GetCamera().GetInverseFar()
                );
            }

            m_Context.Update();
            m_Timer.Tick();
            m_Context.Clear();
        }
    }

} // namespace mt
