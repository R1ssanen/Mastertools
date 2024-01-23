#pragma once

#include "mesh.hpp"
#include "srpch.hpp"

namespace core {

class Object {
 public:
  Object() = default;
  ~Object() = default;

  Object(const std::vector<Mesh>& t_Meshes,
         const glm::vec3& t_Pos,
         const glm::vec3& t_Angle,
         const glm::vec3& t_Scale);

  glm::mat4 GetScale() const;
  glm::mat4 GetRotation(float DeltaTime = 1.f) const;
  glm::mat4 GetTranslation() const;

  const std::vector<Mesh>& GetMeshes() const;

 private:
  std::vector<Mesh> m_Meshes;
  glm::vec3 m_Pos{0.f}, m_Angle{0.f}, m_Scale{1.f};
};

Object LoadAssetOBJ(const std::string& t_Path);

}  // namespace core
