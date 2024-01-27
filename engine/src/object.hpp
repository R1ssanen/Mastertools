#pragma once

#include "mesh.hpp"
#include "srpch.hpp"

namespace core {

class Object {
 public:
  Object() = default;
  ~Object() = default;

  Object(const std::vector<Mesh>& t_Meshes,
         const glm::vec3& t_Pos = glm::vec3(0.f),
         const glm::vec3& t_Angle = glm::vec3(0.f),
         const glm::vec3& t_Scale = glm::vec3(1.f));

  const glm::vec3& GetAngle() const { return m_Angle; }
  void SetAngle(const glm::vec3& t_Angle) { m_Angle = t_Angle; }
  const glm::vec3& GetPos() const { return m_Pos; }

  glm::mat4 GetScale() const;
  glm::mat4 GetRotation(float t_DeltaTime = 0.f) const;
  glm::mat4 GetTranslation() const;

  const std::vector<Mesh>& GetMeshes() const;

 private:
  std::vector<Mesh> m_Meshes;
  glm::vec3 m_Pos{0.f}, m_Angle{0.f}, m_Scale{1.f};
};

std::vector<Mesh> LoadMeshOBJ(const std::string& t_Directory,
                              const std::string& t_Name);

Object rapidobj_LoadAssetOBJ(const std::string& t_Directory,
                             const std::string& t_Name);

}  // namespace core
