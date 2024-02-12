#pragma once

#include "srpch.hpp"
#include "vertex.hpp"
#include "render.hpp"
#include "context.hpp"
#include "texture.hpp"
#include "scene.hpp"
#include "shader.hpp"
#include "settings.hpp"

namespace core {

    class RenderTile {
        public:
            bool IntersectsTriangle(const triangle_t& t_Tri) const {
                auto [Min, Max] = GetBoundingBox(t_Tri);

                return !(Min.x > m.Max.x
                      || Max.x < m.Min.x
                      || Min.y > m.Max.y
                      || Max.y < m.Min.y);
            }

            void AddToQueue(const DrawTri& t_DrawTri) {
                m.RenderQueue.push_back(t_DrawTri);
            }

            void ClearQueue() {
                m.RenderQueue.clear();
            }
            
            const std::vector<DrawTri> GetRenderQueue() const {return m.RenderQueue;}
            const glm::vec2& GetMin() const {return m.Min;}
            const glm::vec2& GetMax() const {return m.Max;}

            void Render(Context& t_Context, const Scene& t_Scene) const {
                for (const DrawTri& DrawTri : m.RenderQueue) {
                    if (GetVisualizeTiles()) {
                        RenderTriBary(t_Context, DrawTri.m_Tri, m.ColorTextureID, t_Scene.GetCamera().GetInverseFar(), OpaqueSTD, m.Min, m.Max);
                    }

                    else {
                        RenderTriBary(t_Context, DrawTri.m_Tri, DrawTri.m_TextureID, t_Scene.GetCamera().GetInverseFar(), OpaqueSTD, m.Min, m.Max);
                    }
                }
            }

            static RenderTile New(int t_MinX, int t_MinY, int t_MaxX, int t_MaxY) {
                return RenderTile(
                    _M{
                        .Min = glm::vec2(t_MinX, t_MinY),
                        .Max = glm::vec2(t_MaxX, t_MaxY),
                        .ColorTextureID = ColorTexture::New(std::to_string(GetRandomColor()), GetRandomColor(), false, false)->GetID()
                    }
                );
            }

        private:
            struct _M {
                std::vector<DrawTri> RenderQueue;
                glm::vec2 Min, Max;
                uint32_t ColorTextureID;
            } m;

            RenderTile(_M&& t_Data) : m{std::move(t_Data)} {}
    };

}
