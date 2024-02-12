#pragma once

#include "camera.hpp"
#include "context.hpp"
#include "mesh.hpp"
#include "srpch.hpp"
#include "texture.hpp"
#include "timer.hpp"
#include "vertex.hpp"

namespace core
{

class Skybox
{
  public:
    void Render(const Camera& t_Camera, Context& t_Context) const;
    void Update();

    static Skybox New(const mesh_t& t_Mesh);

  private:
    triangle_vector_t Transform(const Camera& t_Camera, const Context& t_Context) const;
    void RenderTri(core::Context& t_Context, const triangle_t& t_Tri) const;


    struct _M
    {
        mesh_t Mesh;
        core::Timer Timer;
        double State;
    } m;

    Skybox(_M&& t_Data) : m{std::move(t_Data)} {}
};

Skybox GetDefaultSkybox();

} // namespace core
