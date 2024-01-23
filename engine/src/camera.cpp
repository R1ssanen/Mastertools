#include "camera.hpp"

#include "clipping.hpp"
#include "srpch.hpp"

namespace core {

Camera::Camera(const glm::vec3& t_Pos,
               const glm::vec3& t_Angle,
               float t_FOV,
               float t_Near,
               float t_Far)
    : m_Pos{t_Pos},
      m_Angle{t_Angle},
      m_FOV{t_FOV},
      m_Near{t_Near},
      m_Far{t_Far} {}

glm::mat4 Camera::GetMatView() const {
  glm::vec3 Forward{glm::orientate3(m_Angle) * glm::vec3(0.f, 0.f, -1.f)};
  return glm::lookAtRH(m_Pos, m_Pos + Forward, glm::vec3(0.f, -1.f, 0.f));
}

glm::mat4 Camera::GetMatProjection(float width, float height) const {
  return glm::perspectiveFovRH(m_FOV, width, height, m_Near, m_Far);
}

}  // namespace core
