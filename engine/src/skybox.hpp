#pragma once

#include "camera.hpp"
#include "context.hpp"
#include "mesh.hpp"
#include "srpch.hpp"
#include "timer.hpp"
#include "vertex.hpp"

namespace core {

class Skybox {
 public:
  Skybox(
      const std::string& t_MeshDirectory = "../../resources/meshes/",
      const std::string& t_MeshName = "skybox.obj",
      const std::string& t_TexturePath = "../../resources/skyboxes/night.png");
  ~Skybox() = default;

  void Render(const Camera& t_Camera, Context& t_Context);

 private:
  std::vector<Tri> Transform(const Camera& t_Camera, const Context& t_Context);
  void RenderTri(core::Context& t_Context, const Tri& t_Tri);

  Timer m_Timer;
  double m_State{0.f};

  Mesh m_Mesh;
  std::string m_TexName;
};

}  // namespace core
