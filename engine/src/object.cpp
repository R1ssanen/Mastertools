#include "object.hpp"

#include "mesh.hpp"
#include "srpch.hpp"
#include "texture.hpp"
#include "vertex.hpp"

namespace core {

Object::Object(const mesh_vector_t& t_Meshes,
               const glm::vec3& t_Pos,
               const glm::vec3& t_Angle,
               const glm::vec3& t_Scale)
    : m_Meshes{t_Meshes}, m_Pos{t_Pos}, m_Angle{t_Angle}, m_Scale{t_Scale} {}

glm::mat4 Object::GetScale() const {
  return glm::scale(glm::mat4(1.f), m_Scale);
}

glm::mat4 Object::GetRotation(float t_DeltaTime) const {
  return glm::orientate4(m_Angle + t_DeltaTime);
}

glm::mat4 Object::GetTranslation() const {
  return glm::translate(glm::mat4(1.f), m_Pos);
}

const mesh_vector_t& Object::GetMeshes() const {
  return m_Meshes;
}

}  // namespace core
