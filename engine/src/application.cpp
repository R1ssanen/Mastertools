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

#include "render_tile.hpp"
#include "emitter.hpp"

namespace core
{

void App::Init()
{
    srand(std::chrono::system_clock::now().time_since_epoch().count());
    SDL_AddEventWatch(ActionCallback, this);
    SDL_SetRelativeMouseMode(SDL_TRUE);
}

void App::Run()
{

    std::vector<RenderTile> RenderTiles;
    {
        int Horizontal = 3, Vertical = 3;
        int TileWidth = m_Context.GetWidth() / Horizontal, TileHeight = m_Context.GetHeight() / Vertical;

        for (int H = 0; H < Vertical; H++) {
            for (int W = 0; W < Horizontal; W++) {
                int MinX = W * TileWidth, MinY = H * TileHeight;
                int MaxX = (W + 1) * TileWidth - 1, MaxY = (H + 1) * TileHeight - 1;

                RenderTiles.push_back(RenderTile::New(MinX, MinY, MaxX, MaxY) );
            }
        }
    }

    //emitter_t Emitter = DiskEmitter::New(m_ActiveScene.GetObjects()[0].GetMeshes()[0], 1000, glm::vec3(0.f), 0.25f);

    while (m_Running)
    {
        SDL_PumpEvents();
        m_ActiveScene.Update(m_Timer.GetTimeElapsed() * 0.001f);
        //Emitter->Update(m_Timer.GetTimeElapsed());

        glm::mat4 MatView = m_ActiveScene.GetCamera().GetMatView();
        glm::mat4 MatProjection =
            m_ActiveScene.GetCamera().GetMatProjection(m_Context.GetWidth(), m_Context.GetHeight());
        glm::mat4 MatViewProjection = MatProjection * MatView;

        std::vector<DrawTri> TransparentTris;

        for (const Object& Object : m_ActiveScene.GetObjects())
        {
            glm::mat4 MatModel = Object.GetMatTranslation() * Object.GetMatRotation() * Object.GetMatScale();

            for (const mesh_t& Mesh : Object.GetMeshes())
            {
                texture_t Texture = GetTexture(Mesh->GetTextureID());
                vertex_vector_t TransVertices = Mesh->GetVertices();

                for (Vertex& Vertex : TransVertices)
                {
                    Vertex.m_Pos = MatModel * Vertex.m_Pos;
                    Vertex.m_Normal = glm::normalize(Object.GetMatRotation() * Vertex.m_Normal);

                    for (const PointLight& Light : m_ActiveScene.GetLights())
                    {
                        glm::vec4 LightDirection = glm::normalize(glm::vec4(Light.GetPos(), 1.f) - Vertex.m_Pos);
                        float Lambert = glm::dot(Vertex.m_Normal, LightDirection);

                        float Contribution = Light.GetLighting(Vertex) * ((Lambert + 1.f) * 0.5f);
                        Vertex.m_Light += Contribution;
                        Vertex.m_LightColor += Light.GetColor() * Contribution;
                    }

                    Vertex.m_LightColor = glm::clamp(Vertex.m_LightColor / static_cast<float>(m_ActiveScene.GetLights().size()), glm::vec3(0.f), glm::vec3(1.f));
                    Vertex.m_Pos = MatViewProjection * Vertex.m_Pos;
                }

                for (size_t ID = 0; ID < Mesh->GetIndices().size(); ID += 3)
                {
                    triangle_t UnclippedTri = {TransVertices[ID], TransVertices[ID + 1], TransVertices[ID + 2]};

                    for (triangle_t& Tri :
                         FrustumClipTriangle(UnclippedTri, m_ActiveScene.GetCamera().GetClipFrustum()))
                    {
                        for (Vertex& Vertex : Tri)
                        {
                            Vertex.m_Pos.w = 1.f / Vertex.m_Pos.w;
                            Vertex.m_Pos.x *= Vertex.m_Pos.w;
                            Vertex.m_Pos.y *= Vertex.m_Pos.w;
                        }

                        if (!Texture->IsDoublesided() && ClipspaceBackfaceCull(Tri))
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

                        /*for (RenderTile& Tile : RenderTiles) {
                            if (!Tile.IntersectsTriangle(Tri)) {continue;}

                            else if (Texture->IsTransparent()) {
                                Tile.QueueTransparent(DrawTri{.m_Tri = Tri, .m_Shader = TransparentSTD, .m_TextureID = Texture->GetID(), .m_Alpha = 1.f});
                            }
                            
                            else {
                                Tile.QueueOpaque(DrawTri{.m_Tri = Tri, .m_Shader = OpaqueSTD, .m_TextureID = Texture->GetID(), .m_Alpha = 1.f});
                            }
                        }*/

                        if (Texture->IsTransparent())
                        {
                            if (GetWireframe()) {
                                DrawWireframe(m_Context, Tri, 40, 90, 255);
                            }

                            TransparentTris.push_back(
                                DrawTri{.m_Tri = Tri, .m_Shader = TransparentSTD, .m_TextureID = Texture->GetID(), .m_Alpha = 1.f}
                            );
                        }

                        else
                        {
                            if (GetWireframe()) {
                                DrawWireframe(m_Context, Tri, 255, 255, 60);
                            }

                            RenderTriBary(m_Context.GetBuffer(), DrawTri{.m_Tri = Tri, .m_Shader = OpaqueSTD, .m_TextureID = Texture->GetID(), .m_Alpha = 1.f},
                                          m_ActiveScene.GetCamera().GetInverseFar());
                        }
                    }
                }
            }
        }

        if (m_ActiveScene.GetSkybox().has_value())
        {
            m_ActiveScene.GetSkybox().value().Render(m_ActiveScene.GetCamera(), m_Context.GetBuffer());
        }

/*
        {
            texture_t Texture = GetTexture(Emitter->GetMesh()->GetTextureID());

            for (const Particle& Particle : Emitter->GetParticles()) {
                vertex_vector_t TransVertices = Emitter->GetMesh()->GetVertices();
                
                glm::mat4 MatModel = glm::translate(glm::mat4(1.f), Particle.GetPos()) *
                                     glm::orientate4(Particle.GetAngle()) *
                                     glm::scale(glm::mat4(1.f), Particle.GetScale());

                for (Vertex& Vertex : TransVertices)
                {
                    Vertex.m_Pos = MatModel * Vertex.m_Pos;
                    Vertex.m_Normal = glm::normalize(glm::orientate4(Particle.GetAngle()) * Vertex.m_Normal);

                    for (const PointLight& Light : m_ActiveScene.GetLights())
                    {
                        glm::vec4 LightDirection = glm::normalize(glm::vec4(Light.GetPos(), 1.f) - Vertex.m_Pos);
                        float Lambert = glm::dot(Vertex.m_Normal, LightDirection);

                        Vertex.m_Light += Light.GetLighting(Vertex) * ((Lambert + 1.f) * 0.5f);
                    }

                    Vertex.m_Pos = MatViewProjection * Vertex.m_Pos;
                }

                for (size_t ID = 0; ID < Emitter->GetMesh()->GetIndices().size(); ID += 3)
                {
                    triangle_t UnclippedTri = {TransVertices[ID], TransVertices[ID + 1], TransVertices[ID + 2]};

                    for (triangle_t& Tri :
                            FrustumClipTriangle(UnclippedTri, m_ActiveScene.GetCamera().GetClipFrustum()))
                    {
                        for (Vertex& Vertex : Tri)
                        {
                            Vertex.m_Pos.w = 1.f / Vertex.m_Pos.w;
                            Vertex.m_Pos.x *= Vertex.m_Pos.w;
                            Vertex.m_Pos.y *= Vertex.m_Pos.w;
                            Vertex.m_Pos.z *= Vertex.m_Pos.w;

                            Vertex.m_Pos.x = (Vertex.m_Pos.x + 1.f) * 0.5f * m_Context.GetWidth();
                            Vertex.m_Pos.y = (Vertex.m_Pos.y + 1.f) * 0.5f * m_Context.GetHeight();
                            Vertex.m_UV *= Vertex.m_Pos.w;
                        }

                        if (GetWireframe()) {
                            DrawWireframe(m_Context, Tri, 40, 90, 255);
                        }

                        TransparentTris.push_back(
                            DrawTri{.m_Tri = Tri, .m_Shader = ParticleSTD, .m_TextureID = Texture->GetID(),
                                    .m_Alpha = glm::smoothstep(0.f, 1.f, float(Particle.GetLifetime()) / Particle.GetStartLifetime())}
                        );
                    }
                }
            }

        }
*/

        std::sort(TransparentTris.begin(), TransparentTris.end(), DrawTri::FarToClose);
        for (const DrawTri& DrawTri : TransparentTris) {
            RenderTriBary(m_Context.GetBuffer(), DrawTri, m_ActiveScene.GetCamera().GetInverseFar());
        }

        m_Context.Update();
        m_Timer.Tick();
        m_Context.Clear();
    }
}

} // namespace core
