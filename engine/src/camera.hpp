#pragma once

#include "clipping.hpp"
#include "srpch.hpp"

namespace core {

class Camera {
 public:
  ~Camera() = default;
  Camera() = default;

  glm::mat4 GetMatView() const;
  glm::mat4 GetMatProjection(float width, float height) const;

  const glm::vec3& GetPos() const { return m_Pos; }
  void SetPos(const glm::vec3& t_Val) { m_Pos = t_Val; }
  const Frustum& GetFrustum() const { return m_Frustum; }

  Camera(const glm::vec3& t_Pos,
         const glm::vec3& t_Angle,
         float t_FOV,
         float t_Near,
         float t_Far);

 private:
  glm::vec3 m_Pos{0.f}, m_Angle{0.f};
  float m_FOV{90.f}, m_Near{0.05f}, m_Far{10.f};

  Frustum m_Frustum{
      Plane(glm::vec3(0.f, 0.f, m_Near), glm::vec3(0.f, 0.f, 1.f)),  // near
      Plane(glm::vec3(0.f, 0.f, m_Far), glm::vec3(0.f, 0.f, -1.f)),  // far
  };
};

}  // namespace core
