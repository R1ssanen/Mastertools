#pragma once

#include "clipping.hpp"
#include "srpch.hpp"

#define SENSITIVITY_YAW 1000.f
#define SENSITIVITY_PITCH 1000.f
#define CAM_MAX_VERTICAL_ANGLE 85.f
#define CAMERA_SPEED 0.1f

namespace core {

class Camera {
 public:
  ~Camera() = default;
  Camera() = default;

  glm::mat4 GetMatView() const;
  glm::mat4 GetMatProjection(float width, float height) const;
  glm::mat4 GetMatLookAt(const glm::vec3& t_OriginalPos) const;

  const glm::vec3& GetPos() const { return m_Pos; }
  void SetPos(const glm::vec3& t_Val) { m_Pos = t_Val; }
  const glm::vec3& GetAngle() const { return m_Angle; }
  void SetAngle(const glm::vec3& t_Val) { m_Angle = t_Val; }

  const Frustum& GetFrustum() const { return m_Frustum; }
  float GetInverseFar() const { return 1.f / m_Far; }

  void HandleMovement();
  void HandleRotation();

  Camera(const glm::vec3& t_Pos,
         const glm::vec3& t_Angle,
         float t_FOV,
         float t_Near,
         float t_Far);

 private:
  glm::vec3 m_Pos{0.f}, m_Angle{0.f};
  float m_FOV{90.f}, m_Near{0.05f}, m_Far{100.f};

  Frustum m_Frustum{
      Plane(glm::vec3(0.f, 0.f, m_Near), glm::vec3(0.f, 0.f, 1.f)),  // near
      Plane(glm::vec3(0.f, 0.f, m_Far), glm::vec3(0.f, 0.f, -1.f))   // far
  };
};

}  // namespace core
