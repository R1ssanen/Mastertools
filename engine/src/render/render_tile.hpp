#pragma once

#include "../core/buffer.hpp"
#include "../core/settings.hpp"
#include "../core/triangle.hpp"
#include "../core/vertex.hpp"
#include "../mtpch.hpp"
#include "../resource/scene.hpp"
#include "../resource/shader.hpp"
#include "../resource/texture.hpp"
#include "render.hpp"

namespace mt {

    class RenderTile {
      public:

        b8 IntersectsTriangle(const Triangle& Tri) const {
            auto [Min, Max] = Tri.GetBoundingBox();

            return !(Min.x > m.Max.x || Max.x < m.Min.x || Min.y > m.Max.y || Max.y < m.Min.y);
        }

        void QueueOpaque(const Triangle& Tri) { m.OpaqueQueue.push_back(Tri); }

        void ClearOpaqueQueue() { m.OpaqueQueue.clear(); }

        void RenderOpaque(buffer_t Buffer, const Scene& Scene, std::vector<std::thread>& Pool) {

            Pool.emplace_back([&]() {
                for (Triangle& Tri : this->m.OpaqueQueue) {
                    RenderTriBary(Buffer, Tri, Scene.GetCamera().GetInverseFar());
                }

                m.OpaqueQueue.clear();
            });
        }

        static RenderTile New(i32 MinX, i32 MinY, i32 MaxX, i32 MaxY) {
            return RenderTile(_M{ .Min            = glm::vec2(MinX, MinY),
                                  .Max            = glm::vec2(MaxX, MaxY),
                                  .ColorTextureID = ColorTexture::New(
                                                        std::to_string(GetRandomColor()),
                                                        GetRandomColor(), false, false
                                  )
                                                        ->GetID() });
        }

      private:

        struct _M {
            triangle_vector_t OpaqueQueue;
            glm::vec2         Min, Max;
            u32               ColorTextureID;
        } m;

        RenderTile(_M&& Data) : m{ std::move(Data) } { }
    };

} // namespace mt
