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
    void Render(const Camera& t_Camera, Context& t_Context);

    Skybox() = default;
    Skybox(const mesh_t& t_Mesh) : m_Mesh{t_Mesh} {}

  private:
    triangle_vector_t Transform(const Camera& t_Camera, const Context& t_Context);
    void RenderTri(core::Context& t_Context, const triangle_t& t_Tri);

    mesh_t m_Mesh;
    Timer m_Timer;
    double m_State{0.f};
};

Skybox GetDefaultSkybox();

} // namespace core
