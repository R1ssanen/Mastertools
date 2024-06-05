#pragma once

#include "../mtpch.hpp"
#include "../render/context.hpp"
#include "../resource/mesh.hpp"
#include "../resource/texture.hpp"
#include "camera.hpp"
#include "timer.hpp"
#include "triangle.hpp"
#include "vertex.hpp"

namespace mt {

    class Skybox {
      public:

        void Render(const Camera& Camera, buffer_t Buffer) const;
        void Update();

        Skybox(u32 TextureID);

      private:

        void Transform(
            const Camera& Camera, buffer_t Buffer, triangle_vector_t& o_TransformedTriangles
        ) const;
        void RenderTri(buffer_t Buffer, const Triangle& Tri) const;

        struct _M {
            mesh_t    Mesh;
            texture_t Texture;
            f64       State;
        } m;

        Skybox(_M&& Data) : m{ std::move(Data) } { }
    };

} // namespace mt
