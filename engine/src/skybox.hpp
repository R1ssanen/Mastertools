#pragma once

#include "camera.hpp"
#include "context.hpp"
#include "mesh.hpp"
#include "mtpch.hpp"
#include "texture.hpp"
#include "timer.hpp"
#include "vertex.hpp"
#include "triangle.hpp"

namespace core
{

class Skybox
{
  public:
    void Render(const Camera& Camera, buffer_t Buffer) const;
    void Update();

    static Skybox New(u32 TextureID);

  private:
    void Transform(const Camera& Camera, buffer_t Buffer, triangle_vector_t& o_TransformedTriangles) const;
    void RenderTri(buffer_t Buffer, const Triangle& Tri) const;

    struct _M
    {
        mesh_t Mesh;
        texture_t Texture;
        core::Timer Timer;
        f64 State;
    } m;

    Skybox(_M&& Data) : m{std::move(Data)} {}
};

} // namespace core
