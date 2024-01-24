#pragma once

#include "camera.hpp"
#include "context.hpp"
#include "mesh.hpp"
#include "srpch.hpp"

namespace core {

class Skybox {
 public:
  Skybox(const std::string& t_MeshDirectory = "../../resources/meshes/",
         const std::string& t_MeshName = "skybox.obj",
         const std::string& t_TexturePath = "../../resources/skyboxes/day.png");
  ~Skybox() = default;

  void Render(const Camera& t_Camera, Context& t_Context);

 private:
  std::vector<Tri> Transform(const Camera& t_Camera, const Context& t_Context);

  Mesh m_Mesh;
  std::string m_TexName;
};

}  // namespace core
