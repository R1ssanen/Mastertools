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
        int Horizontal = 4, Vertical = 3;
        int TileWidth = m_Context.GetWidth() / Horizontal, TileHeight = m_Context.GetHeight() / Vertical;

        for (int H = 0; H < Vertical; H++) {
            for (int W = 0; W < Horizontal; W++) {
                int MinX = W * TileWidth, MinY = H * TileHeight;
                int MaxX = (W + 1) * TileWidth - 1, MaxY = (H + 1) * TileHeight - 1;

                RenderTiles.push_back(RenderTile::New(MinX, MinY, MaxX, MaxY) );
            }
        }
    }

    while (m_Running)
    {
        SDL_PumpEvents();
        m_ActiveScene.Update(m_Timer.GetTimeElapsed() * 0.001f);

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
                shader_t Shader = Texture->IsTransparent() ? TransparentSTD : OpaqueSTD;

                vertex_vector_t TransVertices = Mesh->GetVertices();

                for (Vertex& Vertex : TransVertices)
                {
                    Vertex.m_Pos = MatModel * Vertex.m_Pos;
                    Vertex.m_Normal = glm::normalize(Object.GetMatRotation() * Vertex.m_Normal);

                    for (const PointLight& Light : m_ActiveScene.GetLights())
                    {
                        glm::vec4 LightDirection = glm::normalize(glm::vec4(Light.GetPos(), 1.f) - Vertex.m_Pos);
                        float Lambert = glm::dot(Vertex.m_Normal, LightDirection);

                        Vertex.m_Light += Light.GetLighting(Vertex) * ((Lambert + 1.f) * 0.5f);
                    }

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

                        for (RenderTile& Tile : RenderTiles) {
                            if (!Tile.IntersectsTriangle(Tri)) {continue;}

                            Tile.AddToQueue(DrawTri{Tri, Texture->GetID()});
                        }

                        /*if (GetWireframe())
                        {
                            DrawWireframe(m_Context, {Tri[0].m_Pos, Tri[1].m_Pos, Tri[2].m_Pos});
                        }

                        else if (Texture->IsTransparent())
                        {
                            TransparentTris.push_back(DrawTri{Tri, Texture->GetID()});
                        }

                        else
                        {
                            RenderTriBary(m_Context, Tri, Texture, m_ActiveScene.GetCamera().GetInverseFar(),
                                          OpaqueSTD);
                        }*/
                    }
                }
            }
        }

        #pragma omp parallel for schedule(static, 1)
        for (RenderTile& Tile : RenderTiles) {
            Tile.Render(m_Context, m_ActiveScene);
            Tile.ClearQueue();
        }

        if (m_ActiveScene.GetSkybox().has_value())
        {
            m_ActiveScene.GetSkybox().value().Render(m_ActiveScene.GetCamera(), m_Context);
        }


        /*
        std::sort(TransparentTris.begin(), TransparentTris.end(), DrawTri::FarToClose);
        for (const DrawTri& Tri : TransparentTris)
        {
            RenderTriBary(m_Context, Tri.m_Tri, GetTexture(Tri.m_TextureID), m_ActiveScene.GetCamera().GetInverseFar(),
                          TransparentSTD);
        }*/

        m_Context.Update();
        m_Timer.Tick();
        m_Context.Clear();
    }
}

} // namespace core
