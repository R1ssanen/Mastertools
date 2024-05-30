#include "application.hpp"

#include "callbacks.hpp"
#include "culling.hpp"
#include "emitter.hpp"
#include "keys.hpp"
#include "light.hpp"
#include "mesh.hpp"
#include "object.hpp"
#include "render.hpp"
#include "mtpch.hpp"
#include "texture.hpp"
#include "triangle.hpp"
#include "vertex.hpp"

namespace {
    //TTF_Font* s_DefaultFont;
}

namespace core {

    void App::Init() {
        srand(std::chrono::system_clock::now().time_since_epoch().count());
        SDL_AddEventWatch(ActionCallback, this);
        SDL_SetRelativeMouseMode(SDL_TRUE);

        //TTF_Init();
        //s_DefaultFont = TTF_OpenFont("../../resources/arial.ttf", 20);
    }

    void App::Run() {
        while (m_Running) {
            SDL_PumpEvents();

            m_ActiveScene.Update(m_Timer.GetTimeElapsed() * 0.001f);

            glm::mat4         MatView       = m_ActiveScene.GetCamera().GetMatView();
            glm::mat4         MatProjection = m_ActiveScene.GetCamera().GetMatProjection();

            triangle_vector_t OpaqueQueue, TransparentQueue;

            for (const Object& Object : m_ActiveScene.GetObjects()) {
                glm::mat4 MatModel = Object.GetMatModel();

                for (mesh_t Mesh : Object.GetMeshes()) {
                    CullFlag Flag = FrustumCull(m_ActiveScene.GetCamera(), Mesh, Object.GetPos());
                    if (Flag == CullFlag::OUT) { continue; }

                    // texture_t Texture = ColorTexture::New(std::to_string((i32)Flag),
                    // GetDebugCullColor(Flag), false, false);
                    texture_t       Texture       = GetTexture(Mesh->GetTextureID());
                    vertex_vector_t TransVertices = Mesh->GetVertices();

                    for (Vertex& Vertex : TransVertices) {
                        Vertex.m_Pos = MatModel * Vertex.m_Pos;
                        Vertex.m_Normal =
                            glm::normalize(MatModel * glm::vec4(glm::vec3(Vertex.m_Normal), 0.f));

                        for (const PointLight& Light : m_ActiveScene.GetLights()) {
                            glm::vec4 LightDirection =
                                glm::normalize(glm::vec4(Light.GetPos(), 1.f) - Vertex.m_Pos);
                            f32 Lambert = glm::dot(Vertex.m_Normal, LightDirection);

                            f32 Contribution = Light.GetLighting(Vertex) * ((Lambert + 1.f) * 0.5f);
                            Vertex.m_Light += Contribution;
                            Vertex.m_LightColor += Light.GetColor() * Contribution;
                        }

                        Vertex.m_LightColor = glm::clamp(
                            Vertex.m_LightColor /
                                static_cast<f32>(m_ActiveScene.GetLights().size()),
                            glm::vec3(0.f), glm::vec3(1.f)
                        );
                        Vertex.m_Pos = MatView * Vertex.m_Pos;
                    }

                    for (Triangle& Tri : ConstructTriangles(Mesh, TransVertices)) {
                        triangle_vector_t ClippedTriangles;

                        if (Flag == CullFlag::CLIP) {
                            FrustumClipTriangle(
                                Tri, m_ActiveScene.GetCamera().GetFrustum(), ClippedTriangles
                            );
                        } else {
                            ClippedTriangles = { Tri };
                        }

                        for (Triangle& ClippedTri : ClippedTriangles) {
                            for (Vertex& Vertex : ClippedTri.m_Vertices) {
                                Vertex.m_Pos   = MatProjection * Vertex.m_Pos;

                                Vertex.m_Pos.w = 1.f / Vertex.m_Pos.w;
                                Vertex.m_Pos.z *= Vertex.m_Pos.w;
                                Vertex.m_UV *= Vertex.m_Pos.w;

                                Vertex.m_Pos.x = glm::clamp((Vertex.m_Pos.x * Vertex.m_Pos.w) + 1.f, 0.f, 2.f);
                                Vertex.m_Pos.y = glm::clamp((Vertex.m_Pos.y * Vertex.m_Pos.w) + 1.f, 0.f, 2.f);

                                Vertex.m_Pos.x = (Vertex.m_Pos.x * 0.5f) * static_cast<f32>(m_Context.GetWidth() - 1);
                                Vertex.m_Pos.y = (Vertex.m_Pos.y * 0.5f) * static_cast<f32>(m_Context.GetHeight() - 1);
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

#ifndef NDEBUG
                if (GetWireframe()) { DrawWireframe(m_Context, Tri, 255, 255, 60); }
#endif

                RenderTriBary(
                    m_Context.GetBuffer(), Tri, m_ActiveScene.GetCamera().GetInverseFar()
                );
            }

            if (m_ActiveScene.GetSkybox().has_value()) {
                m_ActiveScene.GetSkybox().value().Render(
                    m_ActiveScene.GetCamera(), m_Context.GetBuffer()
                );
            }

            for (const emitter_t& Emitter : m_ActiveScene.GetEmitters()) {

                texture_t Texture     = GetTexture(Emitter->GetMesh()->GetTextureID());
                glm::vec3 CameraAngle = m_ActiveScene.GetCamera().GetAngle();

                for (const Particle& Particle : Emitter->GetParticles()) {

                    CullFlag Flag = FrustumCull(
                        m_ActiveScene.GetCamera(), Emitter->GetMesh(),
                        Emitter->GetPos() + Particle.GetPos()
                    );
                    if (Flag == CullFlag::OUT) { continue; }

                    vertex_vector_t TransVertices = Emitter->GetMesh()->GetVertices();

                    glm::mat4       MatModel = glm::translate(glm::mat4(1.f), Emitter->GetPos()) *
                                         glm::eulerAngleYX(CameraAngle.x, CameraAngle.y) *
                                         // glm::translate(glm::mat4(1.f), Particle.GetPos()) *
                                         glm::scale(glm::mat4(1.f), Particle.GetScale());

                    for (Vertex& Vertex : TransVertices) {
                        Vertex.m_Pos = MatModel * Vertex.m_Pos;
                        Vertex.m_Normal =
                            glm::normalize(glm::orientate4(Particle.GetAngle()) * Vertex.m_Normal);

                        for (const PointLight& Light : m_ActiveScene.GetLights()) {
                            glm::vec4 LightDirection =
                                glm::normalize(glm::vec4(Light.GetPos(), 1.f) - Vertex.m_Pos);
                            f32 Lambert = glm::dot(Vertex.m_Normal, LightDirection);

                            Vertex.m_Light += Light.GetLighting(Vertex) * ((Lambert + 1.f) * 0.5f);
                        }

                        Vertex.m_Pos = MatView * Vertex.m_Pos;
                    }

                    for (const Triangle& Tri :
                         ConstructTriangles(Emitter->GetMesh(), TransVertices)) {
                        triangle_vector_t ClippedTriangles;
                        if (Flag == CullFlag::CLIP) {
                            FrustumClipTriangle(
                                Tri, m_ActiveScene.GetCamera().GetFrustum(), ClippedTriangles
                            );
                        } else {
                            ClippedTriangles = { Tri };
                        }

                        for (Triangle& ClippedTri : ClippedTriangles) {
                            for (Vertex& Vertex : ClippedTri.m_Vertices) {
                                Vertex.m_Pos   = MatProjection * Vertex.m_Pos;

                                Vertex.m_Pos.w = 1.f / Vertex.m_Pos.w;
                                Vertex.m_Pos.x *= Vertex.m_Pos.w;
                                Vertex.m_Pos.y *= Vertex.m_Pos.w;
                                Vertex.m_Pos.z *= Vertex.m_Pos.w;

                                Vertex.m_Pos.x =
                                    (Vertex.m_Pos.x + 1.f) * 0.5f * (m_Context.GetWidth() - 1);
                                Vertex.m_Pos.y =
                                    (Vertex.m_Pos.y + 1.f) * 0.5f * (m_Context.GetHeight() - 1);
                                Vertex.m_UV *= Vertex.m_Pos.w;
                            }

                            ClippedTri.CalculateAverageDepth();
                            ClippedTri.m_Shader = ParticleSTD;
                            // ClippedTri.m_TextureID = ColorTexture::New(std::to_string((i32)Flag),
                            // GetDebugCullColor(Flag), false, false)->GetID();

                            auto Ramp           = [](const core::Particle& Particle) -> f32 {
                                f32 Life = static_cast<f32>(Particle.GetLifetime()) /
                                             Particle.GetStartLifetime();

                                if (Life > 0.65f) { return glm::smoothstep(0.f, 1.f, 1.f - Life); }

                                return glm::smoothstep(0.f, 1.f, Life);
                            };

                            ClippedTri.m_Alpha =
                                Ramp(Particle); // glm::clamp(Ramp(Particle) * 2.f, 0.f, 1.f);
                        }

                        TransparentQueue.insert(
                            TransparentQueue.end(), ClippedTriangles.begin(), ClippedTriangles.end()
                        );
                    }
                }
            }

            std::sort(TransparentQueue.begin(), TransparentQueue.end(), std::greater());
            for (Triangle& Tri : TransparentQueue) {

#ifndef NDEBUG
                if (GetWireframe()) { DrawWireframe(m_Context, Tri, 40, 90, 255, 255); }
#endif

                RenderTriBary(
                    m_Context.GetBuffer(), Tri, m_ActiveScene.GetCamera().GetInverseFar()
                );
            }

            /* std::string PerfText = "scene: " + m_ActiveScene.GetName() + "\n" +
                                   std::to_string(m_Timer.GetAveragePerformance()) + " ms";
            SDL_Surface* PerfSurf = TTF_RenderTexSolid_Wrapped(
                s_DefaultFont, PerfText.c_str(), SDL_Color{ 255, 255, 255, 255 }, 0
            );
            SDL_Rect     PerfRect{ 15, 15, PerfSurf->w, PerfSurf->h };
            SDL_Texture* PerfTexture = SDL_CreateTextureFromSurface(m_Context.Renderer, PerfSurf);

            SDL_RenderCopy(m_Context.Renderer, PerfTexture, nullptr, &PerfRect);

            SDL_FreeSurface(PerfSurf);
            SDL_DestroyTexture(PerfTexture);*/

            m_Context.Update();
            m_Timer.Tick();
            m_Context.Clear();
        }
    }

} // namespace core
