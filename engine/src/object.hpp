#pragma once

#include "mesh.hpp"
#include "srpch.hpp"

namespace core {

class Object {
 public:
  Object() = default;
  ~Object() = default;

  Object(const mesh_vector_t& t_Meshes,
         const glm::vec3& t_Pos = glm::vec3(0.f),
         const glm::vec3& t_Angle = glm::vec3(0.f),
         const glm::vec3& t_Scale = glm::vec3(1.f));

  const glm::vec3& GetAngle() const { return m_Angle; }
  void SetAngle(const glm::vec3& t_Angle) { m_Angle = t_Angle; }
  const glm::vec3& GetPos() const { return m_Pos; }

  glm::mat4 GetScale() const;
  glm::mat4 GetRotation(float t_DeltaTime = 0.f) const;
  glm::mat4 GetTranslation() const;

  const mesh_vector_t& GetMeshes() const;

 private:
  mesh_vector_t m_Meshes;
  glm::vec3 m_Pos{0.f}, m_Angle{0.f}, m_Scale{1.f};
};

}  // namespace core
