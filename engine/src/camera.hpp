#pragma once

#include "clipping.hpp"
#include "srpch.hpp"

#define CAMERA_SPEED 0.1f

namespace core
{

class Camera
{
  public:
    glm::mat4 GetMatView() const;
    glm::mat4 GetMatProjection(float t_Width, float t_Height) const;

    glm::vec3 GetForward() const;
    glm::vec3 GetRight() const;
    glm::vec3 GetUp() const;

    const glm::vec3& GetPos() const { return m_Pos; }
    const glm::vec3& GetAngle() const { return m_Angle; }
    const Frustum& GetFrustum() const { return m_Frustum; }
    float GetInverseFar() const { return 1.f / m_Far; }

    void HandleMovement();
    void HandleRotation();

    Camera() = default;
    Camera(const glm::vec3& t_Pos, const glm::vec3& t_Angle, float t_FOV, float t_Near, float t_Far)
        : m_Pos{t_Pos}, m_Angle{t_Angle}, m_Fov{t_FOV}, m_Near{t_Near}, m_Far{t_Far}
    {
    }

  private:
    glm::vec3 m_Pos, m_Angle;
    float m_Fov, m_Near, m_Far;

    Frustum m_Frustum{
        Plane(glm::vec3(0.f, 0.f, m_Near), glm::vec3(0.f, 0.f, 1.f)), // near
        Plane(glm::vec3(0.f, 0.f, m_Far), glm::vec3(0.f, 0.f, -1.f))  // far
    };
};

Camera GetDefaultCamera();

} // namespace core
