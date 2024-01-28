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
  Skybox(const std::shared_ptr<Mesh>& t_Mesh =
             LoadMeshOBJ("../../engine/builtins/", "skybox.obj")[0]);
  ~Skybox() = default;

  void Render(const Camera& t_Camera, Context& t_Context);

 private:
  std::vector<Tri> Transform(const Camera& t_Camera, const Context& t_Context);
  void RenderTri(core::Context& t_Context, const Tri& t_Tri);

  Timer m_Timer;
  double m_State{0.f};

  std::shared_ptr<Mesh> m_Mesh;
};

}  // namespace core
