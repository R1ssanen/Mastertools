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

            void QueueOpaque(const DrawTri& t_DrawTri) {
                m.OpaqueQueue.push_back(t_DrawTri);
            }

            void QueueTransparent(const DrawTri& t_DrawTri) {
                m.TransparentQueue.push_back(t_DrawTri);
            }

            void ClearOpaqueQueue() {
                m.OpaqueQueue.clear();
            }

            void ClearTransparentQueue() {
                m.TransparentQueue.clear();
            }

            /*std::future<void> RenderOpaque(Context& t_Context, const Scene& t_Scene) {

                //std::sort(m.OpaqueQueue.begin(), m.OpaqueQueue.end(), DrawTri::CloseToFar);

                auto Task = [this] (Context& t_Context, const Scene& t_Scene) mutable {
                    for (DrawTri& DrawTri : this->m.OpaqueQueue) {
                        uint32_t TextureID = GetVisualizeTiles() ? m.ColorTextureID : DrawTri.m_TextureID;
                        RenderTriBary(t_Context, DrawTri, t_Scene.GetCamera().GetInverseFar(), m.Min, m.Max);
                    }

                    m.OpaqueQueue.clear();
                };

                return std::async(std::launch::async, Task, std::ref(t_Context), std::cref(t_Scene));
            }

            void RenderTransparent(Context& t_Context, const Scene& t_Scene) {
                
                std::sort(m.TransparentQueue.begin(), m.TransparentQueue.end(), DrawTri::FarToClose);

                for (const DrawTri& DrawTri : m.TransparentQueue) {
                    
                    uint32_t TextureID = GetVisualizeTiles() ? m.ColorTextureID : DrawTri.m_TextureID;
                    RenderTriBary(t_Context, DrawTri, t_Scene.GetCamera().GetInverseFar(), m.Min, m.Max);
                }
            }*/

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
                std::vector<DrawTri> OpaqueQueue, TransparentQueue;
                glm::vec2 Min, Max;
                uint32_t ColorTextureID;
            } m;

            RenderTile(_M&& t_Data) : m{std::move(t_Data)} {}
    };

}
