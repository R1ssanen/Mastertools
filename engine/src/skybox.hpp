#pragma once

#include "camera.hpp"
#include "context.hpp"
#include "mesh.hpp"
#include "srpch.hpp"
#include "texture.hpp"
#include "timer.hpp"
#include "vertex.hpp"

namespace core {

class Skybox {
 public:
  ~Skybox() = default;

  Skybox(const mesh_t& t_Mesh) : m_Mesh{t_Mesh} {}

  void Render(const Camera& t_Camera, Context& t_Context);

 private:
  std::vector<Tri> Transform(const Camera& t_Camera, const Context& t_Context);
  void RenderTri(core::Context& t_Context, const Tri& t_Tri);

  Timer m_Timer;
  double m_State{0.f};

  mesh_t m_Mesh;
};

}  // namespace core
