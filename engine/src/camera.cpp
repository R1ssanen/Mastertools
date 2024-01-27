#include "camera.hpp"

#include "clipping.hpp"
#include "keys.hpp"
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

void Camera::HandleMovement() {
  if (GetKey(SDL_SCANCODE_W)) {
    glm::vec3 Forward{glm::yawPitchRoll(m_Angle.x, m_Angle.y, m_Angle.z) *
                      glm::vec4(0.f, 0.f, -1.f, 1.f)};
    m_Pos += Forward * CAMERA_SPEED;
  }

  if (GetKey(SDL_SCANCODE_S)) {
    glm::vec3 Forward{glm::yawPitchRoll(m_Angle.x, m_Angle.y, m_Angle.z) *
                      glm::vec4(0.f, 0.f, -1.f, 1.f)};
    m_Pos -= Forward * CAMERA_SPEED;
  }

  if (GetKey(SDL_SCANCODE_A)) {
    glm::vec3 Right{glm::yawPitchRoll(m_Angle.x, m_Angle.y, m_Angle.z) *
                    glm::vec4(1.f, 0.f, 0.f, 1.f)};
    m_Pos -= Right * CAMERA_SPEED;
  }

  if (GetKey(SDL_SCANCODE_D)) {
    glm::vec3 Right{glm::yawPitchRoll(m_Angle.x, m_Angle.y, m_Angle.z) *
                    glm::vec4(1.f, 0.f, 0.f, 1.f)};
    m_Pos += Right * CAMERA_SPEED;
  }

  if (GetKey(SDL_SCANCODE_SPACE)) {
    m_Pos.y -= CAMERA_SPEED;
  }

  if (GetKey(SDL_SCANCODE_LSHIFT)) {
    m_Pos.y += CAMERA_SPEED;
  }
}

void Camera::HandleRotation() {
  auto [DeltaMouseX, DeltaMouseY] = GetDeltaMouse();

  float DeltaAngleYaw{static_cast<float>(DeltaMouseX / SENSITIVITY_YAW)},
      DeltaAnglePitch{static_cast<float>(DeltaMouseY / SENSITIVITY_PITCH)};

  float NewAnglePitch{glm::clamp(m_Angle.y + DeltaAnglePitch,
                                 -glm::radians(CAM_MAX_VERTICAL_ANGLE),
                                 glm::radians(CAM_MAX_VERTICAL_ANGLE))};
  float NewAngleYaw{m_Angle.x - DeltaAngleYaw};

  m_Angle += glm::vec3(NewAngleYaw, NewAnglePitch, m_Angle.z);
  m_Angle *= 0.5f;
}

glm::mat4 Camera::GetMatView() const {
  glm::vec3 Forward{glm::yawPitchRoll(m_Angle.x, m_Angle.y, m_Angle.z) *
                    glm::vec4(0.f, 0.f, -1.f, 1.f)};
  return glm::lookAtRH(m_Pos, m_Pos + Forward, glm::vec3(0.f, 1.f, 0.f));
}

glm::mat4 Camera::GetMatProjection(float width, float height) const {
  return glm::perspectiveFovRH(m_FOV, width, height, m_Near, m_Far);
}

}  // namespace core
